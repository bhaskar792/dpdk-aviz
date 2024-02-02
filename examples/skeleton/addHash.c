#include <stdio.h>
#include "addHash.h"


int main() {
    struct hash_table ht = {0};
    struct pkt_tuple t1 = {0x0a000001, 0x0a000002, 0x1234, 0x5678, 0x06};
    struct pkt_tuple t2 = {0x0a000003, 0x0a000004, 0x4321, 0x8765, 0x06};
    struct pkt_tuple t3 = {0x0a000001, 0x0a000002, 0x1234, 0x5678, 0x06};
    insert(&ht, t1);
    insert(&ht, t2);
    insert(&ht, t3);
    printf("Count for t1: %d\n", get_count(&ht, t1));
    printf("Count for t2: %d\n", get_count(&ht, t2));
    printf("Count for t3: %d\n", get_count(&ht, t3));
    return 0;
}