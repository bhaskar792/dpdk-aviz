#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

 
struct Node {
    int key;
    time_t time;
    struct Node* next;
};
 
void deleteNode(struct Node** head_ref, int key) {
    struct Node* temp = *head_ref, *prev = NULL;
    if (temp != NULL && temp->key == key) {
        *head_ref = temp->next;
        free(temp);
        return;
    }
    while (temp != NULL && temp->key != key) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) {
        return;
    }
    prev->next = temp->next;
    free(temp);
}
 
void insertNode(struct Node** head_ref, int key) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->key = key;
    new_node->time = time(NULL);
    new_node->next = (*head_ref);
    (*head_ref) = new_node;
    
}
 
void removeOldNodes(struct Node** head_ref) {
    struct Node* temp = *head_ref, *prev = NULL;
    time_t now = time(NULL);
    while (temp != NULL) {
        if (now - temp->time > 5) {
            if (prev == NULL) {
                *head_ref = temp->next;
            } else {
                prev->next = temp->next;
            }
            struct Node* to_free = temp;
            temp = temp->next;
            free(to_free);
        } else {
            prev = temp;
            temp = temp->next;
        }
    }
}

// remove oldNodes starting from tail
 
void printList(struct Node* node) {
    while (node != NULL) {
        printf("Key: %d, Time: %ld\n", node->key, node->time);
        node = node->next;
    }
}
 
int main() {
    struct Node* head = NULL;
    struct Node* tail = NULL;
    insertNode(&head, 1);
    insertNode(&head, 2);
    insertNode(&head, 3);
    insertNode(&head, 4);
    insertNode(&head, 5);
    printf("Initial list:\n");
    printList(head);

    // add timer to sleep for 5 seconds
    //print current time
    printf("now: %ld\n", time(NULL));
    sleep(3);
    printf("now: %ld\n", time(NULL));
    removeOldNodes(&head);
    sleep(3);
    printf("now: %ld\n", time(NULL));
    removeOldNodes(&head);
    printf("List after removing old nodes:\n");
    printList(head);
    deleteNode(&head, 3);
    printf("List after deleting node with key 3:\n");
    printList(head);
    return 0;
}