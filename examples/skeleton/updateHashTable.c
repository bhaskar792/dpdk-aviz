
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

void insertNode(struct Node** head_ref, struct Node** tail_ref, int key) {
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
}

void removeOldNodes(struct Node** head_ref, struct Node** tail_ref) {
    struct Node* temp = *tail_ref;
    struct Node* head = *head_ref;
    time_t now = time(NULL);
    while (temp != NULL) 
    {
        if (temp->key == head->key)
        {
            // if (temp != head)
            // {
            //     temp = temp->prev;
            // }
            // else
            // {
            //     break;
            // }
            printf("temp->key: %d\n", temp->key);
                
        }
        if (now - temp->time > 5) 
            {
                if (temp->prev != NULL) 
                    {
                        temp->prev->next = temp->next;
                    } 
                else 
                    {
                        *head_ref = temp->next;
                    }
                // if (temp->next != NULL) 
                //     {
                //         temp->next->prev = temp->prev;
                //     } 
                // else 
                //     {
                //         *tail_ref = temp->prev;
                //     }
                struct Node* to_free = temp;
                temp = temp->prev;
                free(to_free);
            } 
        else 
            {
                break;
            }
    }
}

struct removedKeys* removeOldNodesNew(struct Node** head_ref, struct Node** tail_ref) {
    struct Node* temp = *tail_ref;
    struct Node* head = *head_ref;
    time_t now = time(NULL);
    // int* removed_keys = (int*)malloc(sizeof(int));
    // int removed_keys_count = 0;
    // struct removedKeys* removed_keys_ptr = (struct removedKeys*)malloc(sizeof(struct removedKeys));
    struct removedKeys* removed_keys_ptr = NULL;
    // removed_keys_ptr->next = NULL;
    // removed_keys_ptr->keys = -1;
    struct removedKeys* removed_keys_head;
    while (temp != NULL) 
    {
        if (temp->key == head->key)
        {
            printf("temp->key: %d\n", temp->key);
                
        }
        if (now - temp->time > 5) 
            {
                if (temp->prev != NULL) 
                    {
                        temp->prev->next = temp->next;
                    } 
                else 
                    {
                        *head_ref = temp->next;
                    }
                if (temp->key != head->key)
                    {
                        // removed_keys[removed_keys_count] = temp->key;
                        // removed_keys_count++;      
                        printf("adding key to removed key list: %d\n", temp->key); 
                        // check if removed_keys_ptr is null and allocate memory after checking if key is there
                        if (removed_keys_ptr == NULL)
                        {
                            removed_keys_ptr = (struct removedKeys*)malloc(sizeof(struct removedKeys));
                            removed_keys_ptr->next = NULL;
                            removed_keys_ptr->keys = temp->key;
                            removed_keys_head = removed_keys_ptr;
                        }
                        else
                        {
                            removed_keys_ptr->next = (struct removedKeys*)malloc(sizeof(struct removedKeys));
                            removed_keys_ptr = removed_keys_ptr->next;
                            removed_keys_ptr->next = NULL;
                            removed_keys_ptr->keys = temp->key;
                        }

                        // removed_keys_ptr->keys = temp->key;
                        // removed_keys_ptr->next = (struct removedKeys*)malloc(sizeof(struct removedKeys));
                        // removed_keys_ptr = removed_keys_ptr->next;
                        // removed_keys_ptr->next = NULL;
                        // removed_keys_ptr->keys = -1;
                    } 
                struct Node* to_free = temp;
                temp = temp->prev;
                free(to_free);
            } 
        else 
            {
                break;
            }
    }
    // add -1 to the end of the array
    // removed_keys[removed_keys_count] = -1;
    // print all removed keys with removed keys head
    // while(removed_keys_head->next != NULL)
    // {
    //     printf("Removed key: %d\n", removed_keys_head->keys);
    //     removed_keys_head = removed_keys_head->next;
    // }

    return removed_keys_head;
}
// remove old nodes and return keys of nodes
// that were removed


void printList(struct Node* node) {
    while (node != NULL) {
        printf("Key: %d, Time: %ld\n", node->key, node->time);
        node = node->next;
    }
}

int main() {
    struct Node* head = NULL;
    struct Node* tail = NULL;
    // struct removedKeys* removed_keys = NULL;
    insertNode(&head, &tail, 5);
    insertNode(&head, &tail, 2);
    insertNode(&head, &tail, 3);
    sleep(3);
    insertNode(&head, &tail, 4);
    insertNode(&head, &tail, 5);
    printf("Initial list:\n");
    printList(head);

    printf("now: %ld\n", time(NULL));
    
    // printf("now: %ld\n", time(NULL));
    // removeOldNodes(&head, &tail);
    // print list of keys after removing old nodes

    struct removedKeys* removed_keys_head = removeOldNodesNew(&head, &tail);
    // print removed keys array
    // int i = 0;
    // while (removed_keys[i] != -1) {
    //     printf("Removed key: %d\n", removed_keys[i]);
    //     i++;
    // }
    struct removedKeys* temp = removed_keys_head;
    while (temp != NULL) {
        printf("Removed key: %d\n", temp->keys);
        temp = temp->next;
    }

    sleep(3);
    printf("now: %ld\n", time(NULL));
    // removeOldNodes(&head, &tail);
    removed_keys_head = removeOldNodesNew(&head, &tail);
    // print removed keys array
    // int i = 0;
    // while (removed_keys[i] != -1) {
    //     printf("Removed key: %d\n", removed_keys[i]);
    //     i++;
    // }
    temp = removed_keys_head;
    while (temp != NULL) {
        printf("Removed key: %d\n", temp->keys);
        temp = temp->next;
    }
    printf("List after removing old nodes:\n");
    printList(head);
    deleteNode(&head, &tail, 3);
    printf("List after deleting node with key 3:\n");
    printList(head);
    return 0;
}