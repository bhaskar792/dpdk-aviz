#ifndef __ADD_HASH_H__
#define __ADD_HASH_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define MAX_TUPLES 500009

struct pkt_tuple
{
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t proto;
};

struct packet_count
{
    struct pkt_tuple t;
    uint32_t count;
    uint32_t total_rx_bytes;
};

struct hash_table
{
    struct packet_count* table[MAX_TUPLES];
};

// int hash(struct pkt_tuple t);
// int tuples_are_equal(struct pkt_tuple a, struct pkt_tuple b);
// int insert(struct hash_table* ht, struct pkt_tuple t, uint32_t rx_bytes);
// int remove_index(struct hash_table* ht, int index);
// uint32_t get_count(struct hash_table* ht, struct pkt_tuple t);
// uint32_t get_size(struct hash_table* ht, struct pkt_tuple t);
// char* write_ipv4_addr(uint32_t ip);
// void save_hash_table(struct hash_table* ht, char* filename);


#endif


