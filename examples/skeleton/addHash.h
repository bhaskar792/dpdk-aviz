// #ifndef __ADD_HASH_H__
// #define __ADD_HASH_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define MAX_TUPLES 500009

struct pkt_tuple {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t proto;
};

struct packet_count {
    struct pkt_tuple t;
    uint32_t count;
    uint32_t total_rx_bytes;
};

struct hash_table {
    struct packet_count *table[MAX_TUPLES];
};

int hash(struct pkt_tuple t) {
    return (t.src_ip *7907 ^ t.dst_ip *7901 ^ (t.src_port *7883 + t.dst_port*7879 + t.proto*7919)) % MAX_TUPLES;
}
int tuples_are_equal(struct pkt_tuple a, struct pkt_tuple b) {
    
    return a.src_ip == b.src_ip && a.dst_ip == b.dst_ip && a.src_port == b.src_port && a.dst_port == b.dst_port && a.proto == b.proto;

}
int insert(struct hash_table *ht, struct pkt_tuple t, uint32_t rx_bytes) {
    int index = hash(t);
    if (ht->table[index] == NULL) {
        ht->table[index] = malloc(sizeof(struct packet_count));
        ht->table[index]->t = t;
        ht->table[index]->count = 1;
        ht->table[index]->total_rx_bytes = rx_bytes;
    } else {
        // if (memcmp(&ht->table[index]->t, &t, sizeof(struct pkt_tuple)) == 0) {
        if (tuples_are_equal(ht->table[index]->t, t)) {
            ht->table[index]->count++;
            ht->table[index]->total_rx_bytes += rx_bytes;
        } else {
            printf("Collision detected in insert!\n");
            // print all values of the hash table
            for (int i = 0; i < MAX_TUPLES; i++) {
                if (ht->table[i] != NULL) {
                    printf("Index: %d, src_ip: %x, dst_ip: %x, src_port: %d, dst_port: %d, proto: %d, count: %d\n", i, ht->table[i]->t.src_ip, ht->table[i]->t.dst_ip, ht->table[i]->t.src_port, ht->table[i]->t.dst_port, ht->table[i]->t.proto, ht->table[i]->count);
                }
            }
            // print t
            printf("src_ip: %x, dst_ip: %x, src_port: %d, dst_port: %d, proto: %d\n", t.src_ip, t.dst_ip, t.src_port, t.dst_port, t.proto);
            printf("index %d\n",index);
            return -1;
        }
    }
    return 0;
}

uint32_t get_count(struct hash_table *ht, struct pkt_tuple t) {
    int index = hash(t);
    if (ht->table[index] == NULL) {
        return 0;
    } else {
        // if (memcmp(&ht->table[index]->t, &t, sizeof(struct pkt_tuple)) == 0) {
        if (tuples_are_equal(ht->table[index]->t, t)) {
            return ht->table[index]->count;
        } else {
            printf("Collision detected!\n");
            return 0;
        }
    }
}

uint32_t get_size(struct hash_table *ht, struct pkt_tuple t) {
    int index = hash(t);
    if (ht->table[index] == NULL) {
        return 0;
    } else {
        // if (memcmp(&ht->table[index]->t, &t, sizeof(struct pkt_tuple)) == 0) {
        if (tuples_are_equal(ht->table[index]->t, t)) {
            return ht->table[index]->total_rx_bytes;
        } else {
            printf("Collision detected!\n");
            return 0;
        }
    }
}
// convert hex to IP in dot form
char *write_ipv4_addr(uint32_t ip) {
    char *dot_ip = malloc(16);
    sprintf(dot_ip, "%d.%d.%d.%d", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
    return dot_ip;
}
// save hash table data to file
void save_hash_table(struct hash_table *ht, char *filename) {
    // delete the file if it exists
    // remove(filename);
    FILE *f = fopen (filename,
                    "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    for (int i = 0; i < MAX_TUPLES; i++) {
        if (ht->table[i] != NULL) {
            
            fprintf(f, "src IP: %s dst IP: %s src port: %d dst port: %d proto: %d pkt_count: %d rx_bytes: %d index: %d\n", write_ipv4_addr(ht->table[i]->t.src_ip) , write_ipv4_addr(ht->table[i]->t.dst_ip), ht->table[i]->t.src_port, ht->table[i]->t.dst_port, ht->table[i]->t.proto, ht->table[i]->count, ht->table[i]->total_rx_bytes, i);
        }
    }
    fclose(f);
}


// #endif
// int main() {
//     struct hash_table ht = {0};
//     struct pkt_tuple t1 = {0x0a000001, 0x0a000002, 0x1234, 0x5678, 0x06};
//     struct pkt_tuple t2 = {0x0a000003, 0x0a000004, 0x4321, 0x8765, 0x06};
//     struct pkt_tuple t3 = {0x0a000001, 0x0a000002, 0x1234, 0x5678, 0x06};
//     insert(&ht, t1);
//     insert(&ht, t2);
//     insert(&ht, t3);
//     printf("Count for t1: %d\n", get_count(&ht, t1));
//     printf("Count for t2: %d\n", get_count(&ht, t2));
//     printf("Count for t3: %d\n", get_count(&ht, t3));
//     return 0;
// }
