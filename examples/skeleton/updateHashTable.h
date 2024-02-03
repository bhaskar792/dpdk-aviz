
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct Node {
    int key;
    time_t time;
    struct Node* next;
    struct Node* prev;
};

// struct Node* head = NULL;
// struct Node* tail = NULL;

struct removedKeys {
    int keys;
    struct removedKeys* next;    
};

void deleteNode(struct Node** head_ref, struct Node** tail_ref, int key) {
    struct Node* temp = *head_ref;
    if (temp != NULL && temp->key == key) {
        *head_ref = temp->next;
        if (*head_ref != NULL) {
            (*head_ref)->prev = NULL;
        } else {
            *tail_ref = NULL;
        }
        free(temp);
        return;
    }
    while (temp != NULL && temp->key != key) {
        temp = temp->next;
    }
    if (temp == NULL) {
        return;
    }
    if (temp->next != NULL) {
        temp->next->prev = temp->prev;
    }
    if (temp->prev != NULL) {
        temp->prev->next = temp->next;
    }
    free(temp);
}

// delete all nodes of the given key
void deleteAllNodes(struct Node** head_ref, struct Node** tail_ref, int key) {
    struct Node* temp = *head_ref;
    while (temp != NULL) {
        if (temp->key == key) {
            deleteNode(head_ref, tail_ref, key);
        }
        temp = temp->next;
    }
}

void insertNode(struct Node** head_ref, struct Node** tail_ref, int key) {
    deleteNode(head_ref, tail_ref, key);
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->key = key;
    new_node->time = time(NULL);
    new_node->next = (*head_ref);
    new_node->prev = NULL;
    if (*head_ref != NULL) {
        (*head_ref)->prev = new_node;
    }
    (*head_ref) = new_node;
    if (*tail_ref == NULL) {
        *tail_ref = new_node;
    }
    // printf("INSERT head key  %ld", (*head_ref)->time);
    // printf("tail key %ld\n\n", (*tail_ref)->time);
    // print all keys
    // printf("all Key in list:");
    // while(new_node != NULL) {
    //     printf(" %d ||", new_node->key);
    //     new_node = new_node->next;
    // }
}

struct removedKeys* removeOldNodes(struct Node** head_ref, struct Node** tail_ref) {
    struct Node* temp = *tail_ref;
    struct Node* head = *head_ref;
    // printf("REMOVE head key %ld", (*head_ref)->time);
    // printf("tail key %ld\n\n", (*tail_ref)->time);
    time_t now = time(NULL);
    struct removedKeys* removed_keys_ptr = NULL;
    struct removedKeys* removed_keys_head = NULL;
    if (*head_ref == *tail_ref)
        {
            return removed_keys_head;
        }
    while (temp != NULL) 
    {
        // if (temp->key == head->key)
        // {
        //     printf("temp->key: %d\n", temp->key);
                
        // }
        if (now - temp->time > 60) 
            {
                if (temp->prev != NULL) 
                    {
                        temp->prev->next = temp->next;
                    } 
                // else 
                //     {
                //         *head_ref = temp->next;
                //     }
                if (temp->key != head->key)
                    {
                        if (removed_keys_ptr == NULL)
                        {
                            removed_keys_ptr = (struct removedKeys*)malloc(sizeof(struct removedKeys));
                            removed_keys_head = removed_keys_ptr;
                        }
                        else
                        {
                            removed_keys_ptr->next = (struct removedKeys*)malloc(sizeof(struct removedKeys));
                            removed_keys_ptr = removed_keys_ptr->next;
                        }
                        removed_keys_ptr->next = NULL;
                        removed_keys_ptr->keys = temp->key;
                    } 
                struct Node* to_free = temp;
                temp = temp->prev;
                if (to_free == *tail_ref) 
                    {
                    *tail_ref = temp;
                    }
                free(to_free);
            } 
        else 
            {
                break;
            }
    }

    return removed_keys_head;
}


void printList(struct Node* node) {
    while (node != NULL) {
        printf("Key: %d, Time: %ld\n", node->key, node->time);
        node = node->next;
    }
}

// int main() {
//     struct Node* head = NULL;
//     struct Node* tail = NULL;
//     insertNode(&head, &tail, 5);
//     insertNode(&head, &tail, 2);
//     insertNode(&head, &tail, 3);
//     sleep(3);
//     insertNode(&head, &tail, 4);
//     insertNode(&head, &tail, 5);
//     printf("Initial list:\n");
//     printList(head);

//     printf("now: %ld\n", time(NULL));
    

//     struct removedKeys* removed_keys_head = removeOldNodesNew(&head, &tail);
//     // print removed keys array
//     // int i = 0;
//     // while (removed_keys[i] != -1) {
//     //     printf("Removed key: %d\n", removed_keys[i]);
//     //     i++;
//     // }
//     struct removedKeys* temp = removed_keys_head;
//     while (temp != NULL) {
//         printf("Removed key: %d\n", temp->keys);
//         temp = temp->next;
//     }

//     sleep(3);
//     printf("now: %ld\n", time(NULL));
//     // removeOldNodes(&head, &tail);
//     removed_keys_head = removeOldNodesNew(&head, &tail);
//     // print removed keys array
//     // int i = 0;
//     // while (removed_keys[i] != -1) {
//     //     printf("Removed key: %d\n", removed_keys[i]);
//     //     i++;
//     // }
//     temp = removed_keys_head;
//     while (temp != NULL) {
//         printf("Removed key: %d\n", temp->keys);
//         temp = temp->next;
//     }
//     printf("List after removing old nodes:\n");
//     printList(head);
//     deleteNode(&head, &tail, 3);
//     printf("List after deleting node with key 3:\n");
//     printList(head);
//     return 0;
// }