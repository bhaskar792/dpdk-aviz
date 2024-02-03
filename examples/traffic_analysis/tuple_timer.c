#include "tuple_timer.h"

void deleteNode(struct Node** head_ref, struct Node** tail_ref, int key)
{
	struct Node* temp = *head_ref;
	if (temp != NULL && temp->key == key)
	{
		*head_ref = temp->next;
		if (*head_ref != NULL)
		{
			(*head_ref)->prev = NULL;
		}
		else
		{
			*tail_ref = NULL;
		}
		free(temp);
		return;
	}
	while (temp != NULL && temp->key != key)
	{
		temp = temp->next;
	}
	if (temp == NULL)
	{
		return;
	}
	if (temp->next != NULL)
	{
		temp->next->prev = temp->prev;
	}
	if (temp->prev != NULL)
	{
		temp->prev->next = temp->next;
	}
	free(temp);
}

void deleteAllNodes(struct Node** head_ref, struct Node** tail_ref, int key)
{
	struct Node* temp = *head_ref;
	while (temp != NULL)
	{
		if (temp->key == key)
		{
			deleteNode(head_ref, tail_ref, key);
		}
		temp = temp->next;
	}
}

void insertNode(struct Node** head_ref, struct Node** tail_ref, int key)
{
	deleteNode(head_ref, tail_ref, key);
	struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
	new_node->key = key;
	new_node->time = time(NULL);
	new_node->next = (*head_ref);
	new_node->prev = NULL;
	if (*head_ref != NULL)
	{
		(*head_ref)->prev = new_node;
	}
	(*head_ref) = new_node;
	if (*tail_ref == NULL)
	{
		*tail_ref = new_node;
	}
}

struct removedKeys* removeOldNodes(struct Node** head_ref, struct Node** tail_ref)
{
	struct Node* temp = *tail_ref;
	struct Node* head = *head_ref;
	time_t now = time(NULL);
	struct removedKeys* removed_keys_ptr = NULL;
	struct removedKeys* removed_keys_head = NULL;
	struct Node* to_free;
	if (*head_ref == *tail_ref)
	{
		return removed_keys_head;
	}
	while (temp != NULL)
	{
		if (now - temp->time > 60)
		{
			if (temp->prev != NULL)
			{
				temp->prev->next = temp->next;
			}
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
			to_free = temp;
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


void printList(struct Node* node)
{
	while (node != NULL)
	{
		printf("Key: %d, Time: %ld\n", node->key, node->time);
		node = node->next;
	}
}
