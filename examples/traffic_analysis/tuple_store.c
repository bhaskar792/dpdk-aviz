#include "tuple_store.h"

int hash(struct pkt_tuple t)
{
    return (t.src_ip * 7907 ^ t.dst_ip * 7901 ^ (t.src_port * 7883 + t.dst_port * 7879 + t.proto * 7919)) % MAX_TUPLES;
}
int tuples_are_equal(struct pkt_tuple a, struct pkt_tuple b)
{

    return a.src_ip == b.src_ip && a.dst_ip == b.dst_ip && a.src_port == b.src_port && a.dst_port == b.dst_port && a.proto == b.proto;

}
int insert(struct hash_table* ht, struct pkt_tuple t, uint32_t rx_bytes)
{
    int index = hash(t);
    if (ht->table[index] == NULL)
    {
        ht->table[index] = malloc(sizeof(struct packet_count));
        ht->table[index]->t = t;
        ht->table[index]->count = 1;
        ht->table[index]->total_rx_bytes = rx_bytes;
    }
    else
    {
        if (tuples_are_equal(ht->table[index]->t, t))
        {
            ht->table[index]->count++;
            ht->table[index]->total_rx_bytes += rx_bytes;
        }
        else
        {
            printf("Collision detected in insert!\n");
            for (int i = 0; i < MAX_TUPLES; i++)
            {
                if (ht->table[i] != NULL)
                {
                    printf("Index: %d, src_ip: %x, dst_ip: %x, src_port: %d, dst_port: %d, proto: %d, count: %d\n", i, ht->table[i]->t.src_ip, ht->table[i]->t.dst_ip, ht->table[i]->t.src_port, ht->table[i]->t.dst_port, ht->table[i]->t.proto, ht->table[i]->count);
                }
            }
            printf("src_ip: %x, dst_ip: %x, src_port: %d, dst_port: %d, proto: %d\n", t.src_ip, t.dst_ip, t.src_port, t.dst_port, t.proto);
            printf("index %d\n", index);
            return -1;
        }
    }
    return index;
}

int remove_index(struct hash_table* ht, int index)
{
    if (ht->table[index] != NULL)
    {
        free(ht->table[index]);
        ht->table[index] = NULL;
        return 0;
    }
    else
    {
        return -1;
    }

}
uint32_t get_count(struct hash_table* ht, struct pkt_tuple t)
{
    int index = hash(t);
    if (ht->table[index] == NULL)
    {
        return 0;
    }
    else
    {
        if (tuples_are_equal(ht->table[index]->t, t))
        {
            return ht->table[index]->count;
        }
        else
        {
            printf("Collision detected!\n");
            return 0;
        }
    }
}

uint32_t get_size(struct hash_table* ht, struct pkt_tuple t)
{
    int index = hash(t);
    if (ht->table[index] == NULL)
    {
        return 0;
    }
    else
    {
        if (tuples_are_equal(ht->table[index]->t, t))
        {
            return ht->table[index]->total_rx_bytes;
        }
        else
        {
            printf("Collision detected!\n");
            return 0;
        }
    }
}

char* write_ipv4_addr(uint32_t ip)
{
    char* dot_ip = malloc(16);
    sprintf(dot_ip, "%d.%d.%d.%d", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
    return dot_ip;
}

void save_hash_table(struct hash_table* ht, char* filename)
{
    FILE* f = fopen(filename, "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    for (int i = 0; i < MAX_TUPLES; i++)
    {
        if (ht->table[i] != NULL)
        {

            fprintf(f, "src IP: %s dst IP: %s src port: %d dst port: %d proto: %d pkt_count: %d rx_bytes: %d index: %d\n", write_ipv4_addr(ht->table[i]->t.src_ip), write_ipv4_addr(ht->table[i]->t.dst_ip), ht->table[i]->t.src_port, ht->table[i]->t.dst_port, ht->table[i]->t.proto, ht->table[i]->count, ht->table[i]->total_rx_bytes, i);
        }
    }
    fclose(f);
}

