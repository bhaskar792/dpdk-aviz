
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "updateHashTable.h"

int main() {
    struct Node* head = NULL;
    struct Node* tail = NULL;
    insertNode(&head, &tail, 5);
    insertNode(&head, &tail, 2);
    insertNode(&head, &tail, 3);
    sleep(1);
    insertNode(&head, &tail, 4);
    insertNode(&head, &tail, 5);
//     printf("Initial list:\n");
//     printList(head);

//     printf("now: %ld\n", time(NULL));
    

//     struct removedKeys* removed_keys_head = removeOldNodes(&head, &tail);
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

    // sleep(1);
//     printf("now: %ld\n", time(NULL));
//     // removeOldNodes(&head, &tail);
//     removed_keys_head = removeOldNodes(&head, &tail);
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
    // deleteAllNodes(&head, &tail, 5);
    printf("List after deleting node with key 3:\n");
    printList(head);
    return 0;
}