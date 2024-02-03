
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct Node
{
	int key;
	time_t time;
	struct Node* next;
	struct Node* prev;
};

struct removedKeys
{
	int keys;
	struct removedKeys* next;
};
