/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2015 Intel Corporation
 */

#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <netinet/in.h>
#include "store_tuple.h"
#include "hash_timer.h"

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32
#define LOGGING 0

struct hash_table ht = { 0 };
struct Node* head = NULL;
struct Node* tail = NULL;
#define RTE_BE_TO_CPU_16(be_16_v) \
	(uint16_t) ((((be_16_v) & 0xFF) << 8) | ((be_16_v) >> 8))


static inline int
port_init(uint16_t port, struct rte_mempool* mbuf_pool)
{
	struct rte_eth_conf port_conf;
	const uint16_t rx_rings = 1, tx_rings = 1;
	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;
	int retval;
	uint16_t q;
	struct rte_eth_dev_info dev_info;
	struct rte_eth_txconf txconf;

	if (!rte_eth_dev_is_valid_port(port))
		return -1;

	memset(&port_conf, 0, sizeof(struct rte_eth_conf));

	retval = rte_eth_dev_info_get(port, &dev_info);
	if (retval != 0) {
		printf("Error during getting device (port %u) info: %s\n",
			port, strerror(-retval));
		return retval;
	}

	if (dev_info.tx_offload_capa & RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE)
		port_conf.txmode.offloads |=
		RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE;

	/* Configure the Ethernet device. */
	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
	if (retval != 0)
		return retval;

	retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
	if (retval != 0)
		return retval;

	/* Allocate and set up 1 RX queue per Ethernet port. */
	for (q = 0; q < rx_rings; q++) {
		retval = rte_eth_rx_queue_setup(port, q, nb_rxd,
			rte_eth_dev_socket_id(port), NULL, mbuf_pool);
		if (retval < 0)
			return retval;
	}

	txconf = dev_info.default_txconf;
	txconf.offloads = port_conf.txmode.offloads;
	/* Allocate and set up 1 TX queue per Ethernet port. */
	for (q = 0; q < tx_rings; q++) {
		retval = rte_eth_tx_queue_setup(port, q, nb_txd,
			rte_eth_dev_socket_id(port), &txconf);
		if (retval < 0)
			return retval;
	}

	/* Starting Ethernet port. 8< */
	retval = rte_eth_dev_start(port);
	/* >8 End of starting of ethernet port. */
	if (retval < 0)
		return retval;

	/* Display the port MAC address. */
	struct rte_ether_addr addr;
	retval = rte_eth_macaddr_get(port, &addr);
	if (retval != 0)
		return retval;

	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
		" %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
		port, RTE_ETHER_ADDR_BYTES(&addr));

	/* Enable RX in promiscuous mode for the Ethernet device. */
	retval = rte_eth_promiscuous_enable(port);
	/* End of setting RX port in promiscuous mode. */
	if (retval != 0)
		return retval;

	return 0;
}
/* >8 End of main functional part of port initialization. */

static void
ipv4_addr_to_dot(uint32_t be_ipv4_addr, char* buf)
{
	uint32_t ipv4_addr;

	ipv4_addr = rte_be_to_cpu_32(be_ipv4_addr);
	sprintf(buf, "%d.%d.%d.%d", (ipv4_addr >> 24) & 0xFF,
		(ipv4_addr >> 16) & 0xFF, (ipv4_addr >> 8) & 0xFF,
		ipv4_addr & 0xFF);
}

static void
ipv4_addr_dump(const char* what, uint32_t be_ipv4_addr)
{
	char buf[16];

	ipv4_addr_to_dot(be_ipv4_addr, buf);
	if (what)
		printf("%s", what);
	printf("%s", buf);
}

static __rte_noreturn void
lcore_main(void)
{
	uint16_t port;
	uint16_t i;
	struct rte_mbuf* pkt;
	struct rte_ether_hdr* eth_h;
	struct rte_ipv4_hdr* ip_h;
	struct rte_ether_addr eth_addr;
	struct rte_tcp_hdr* tcp_h;
	struct rte_udp_hdr* udp_h;
	uint16_t eth_type;
	uint32_t ipv4_src_addr;
	uint32_t ipv4_dst_addr;
	uint8_t proto;
	uint16_t src_port;
	uint16_t dst_port;
	union
	{
		struct rte_ether_hdr* eth;
		struct rte_ipv4_hdr* ipv4;
		struct rte_tcp_hdr* tcp;
		struct rte_udp_hdr* udp;
		uint8_t* byte;
	} h;

	RTE_ETH_FOREACH_DEV(port)
		if (rte_eth_dev_socket_id(port) >= 0 &&
			rte_eth_dev_socket_id(port) !=
			(int)rte_socket_id())
			printf("WARNING, port %u is on remote NUMA node to "
				"polling thread.\n\tPerformance will "
				"not be optimal.\n", port);

	printf("\nCore %u forwarding packets. [Ctrl+C to quit]\n",
		rte_lcore_id());

	for (;;)
	{
		RTE_ETH_FOREACH_DEV(port)
		{

			struct rte_mbuf* bufs[BURST_SIZE];
			struct pkt_tuple* tuple_data;
			const uint16_t nb_rx = rte_eth_rx_burst(port, 0,
				bufs, BURST_SIZE);

			if (unlikely(nb_rx == 0))
				continue;
			for (i = 0; i < nb_rx; i++)
			{
				pkt = bufs[i];

				eth_h = rte_pktmbuf_mtod(pkt, struct rte_ether_hdr*);
				eth_type = RTE_BE_TO_CPU_16(eth_h->ether_type);
				if (eth_type == RTE_ETHER_TYPE_IPV4)
				{
					ip_h = (struct rte_ipv4_hdr*)((char*)eth_h + sizeof(struct rte_ether_hdr));
					tuple_data = malloc(sizeof(struct pkt_tuple));
					if (LOGGING == 1)
					{
						ipv4_addr_dump("  IPV4: src=", ip_h->src_addr);
						ipv4_addr_dump(" dst=", ip_h->dst_addr);
						printf("Protocol: %d\n", ip_h->next_proto_id);
					}

					if (ip_h->next_proto_id == IPPROTO_TCP)
					{
						tcp_h = (struct rte_tcp_hdr*)((char*)ip_h + sizeof(struct rte_ipv4_hdr));
						src_port = RTE_BE_TO_CPU_16(tcp_h->src_port);
						dst_port = RTE_BE_TO_CPU_16(tcp_h->dst_port);

						tuple_data->src_ip = ip_h->src_addr;
						tuple_data->dst_ip = ip_h->dst_addr;
						tuple_data->src_port = src_port;
						tuple_data->dst_port = dst_port;
						tuple_data->proto = ip_h->next_proto_id;
					}
					else if (ip_h->next_proto_id == IPPROTO_UDP)
					{
						udp_h = (struct rte_udp_hdr*)((char*)ip_h + sizeof(struct rte_ipv4_hdr));
						src_port = RTE_BE_TO_CPU_16(udp_h->src_port);
						dst_port = RTE_BE_TO_CPU_16(udp_h->dst_port);
						tuple_data->src_ip = ip_h->src_addr;
						tuple_data->dst_ip = ip_h->dst_addr;
						tuple_data->src_port = src_port;
						tuple_data->dst_port = dst_port;
						tuple_data->proto = ip_h->next_proto_id;
					}

				}
				if (tuple_data != NULL && tuple_data->src_port == 1)
				{
					int index = insert(&ht, *tuple_data, rte_pktmbuf_pkt_len(pkt));
					if (LOGGING == 2)
					{
						printf("src ip: %x\n", tuple_data->src_ip);
						printf("dst ip: %x\n", tuple_data->dst_ip);
						printf("src port: %d\n", tuple_data->src_port);
						printf("dst port: %d\n", tuple_data->dst_port);
						printf("proto: %d\n", tuple_data->proto);
						printf("index: %d\n", index);
						printf("Count for tuple: %d\n", get_count(&ht, *tuple_data));
						printf("size of bytes rx: %d\n", get_size(&ht, *tuple_data));
					}
					
					insertNode(&head, &tail, index);
					if (LOGGING == 3)
					{
						printList(head);
					}
					struct removedKeys* remove_hash = removeOldNodes(&head, &tail);
					struct removedKeys* temp = remove_hash;
					if (LOGGING == 3)
					{
						printf("After removing old nodes\n");
						printList(head);
					}

					while (temp != NULL) 
					{
						if (LOGGING == 3)
						{
							printf("Removed key: %d || src ip: %x\n", temp->keys, ht.table[temp->keys]->t.src_ip);
						}
						remove_index(&ht, temp->keys);
						temp = temp->next;
					}
					save_hash_table(&ht, "hash_table.txt");
				}
				tuple_data = NULL;
			}
		}
	}
}

int
main(int argc, char* argv[])
{
	struct rte_mempool* mbuf_pool;
	unsigned nb_ports;
	uint16_t portid;

	/* Initializion the Environment Abstraction Layer (EAL). 8< */
	int ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");
	/* >8 End of initialization the Environment Abstraction Layer (EAL). */

	argc -= ret;
	argv += ret;

	/* Check that there is an even number of ports to send/receive on. */
	nb_ports = rte_eth_dev_count_avail();
	// if (nb_ports < 2 || (nb_ports & 1))
	// 	rte_exit(EXIT_FAILURE, "Error: number of ports must be even\n");

	/* Creates a new mempool in memory to hold the mbufs. */

	/* Allocates mempool to hold the mbufs. 8< */
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * nb_ports,
		MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
	/* >8 End of allocating mempool to hold mbuf. */

	if (mbuf_pool == NULL)
		rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");

	/* Initializing all ports. 8< */
	RTE_ETH_FOREACH_DEV(portid)
		if (port_init(portid, mbuf_pool) != 0)
			rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu16 "\n",
				portid);
	/* >8 End of initializing all ports. */

	if (rte_lcore_count() > 1)
		printf("\nWARNING: Too many lcores enabled. Only 1 used.\n");

	/* Call lcore_main on the main core only. Called on single lcore. 8< */
	lcore_main();
	/* >8 End of called on single lcore. */

	/* clean up the EAL */
	rte_eal_cleanup();

	return 0;
}
