#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "SkipList.h"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

SkipListNode *skipListNodeCreate(int height) {
	SkipListNode *nodey;
	int level;
	nodey = malloc(sizeof(SkipListNode));

	if (nodey == NULL)
		return NULL;

	nodey->data = -1;

	nodey->height = height;
	nodey->next = malloc(sizeof(SkipListNode*) * height);
	if (nodey->next == NULL) {
		free(nodey);
		return NULL;
	}
	for (level = 0; level < height; level++)
		nodey->next[level] = NULL;

	return nodey;
}

SkipListNode *skipListNodeDestroy(SkipListNode *nodey) {
	if (nodey == NULL)
		return NULL;

	free(nodey->next);
	free(nodey);
	return NULL;
}

SkipListNode *skipListNodeGetNext(SkipListNode *nodey, int level) {
	if (nodey == NULL)
		return NULL;
	if (level < 0 || level >= nodey->height)
		return;
	return nodey->next[level];
}

void skipListNodeSetNext(SkipListNode *nodey, int level, SkipListNode *newNext) {
	if (nodey == NULL)
		return;
	if (level < 0 || level >= nodey->height)
		return;
	nodey->next[level] = newNext;
}

void skipListNodeGrow(SkipListNode *nodey) {
	SkipListNode **next;
	int level;

	// Create a new array of pointers that is longer
	next = malloc(sizeof(SkipListNode*) * (nodey->height + 1));
	if (next == NULL)
		return;

	// Copy existing pointers over
	for (level = 0; level < nodey->height; level++)
		next[level] = nodey->next[level];

	// Add a NULL reference to the top
	next[level] = NULL;

	// Free old array
	free(nodey->next);

	// Put changes in place for growth
	nodey->next = next;
	nodey->height++;
}

int skipListNodeMaybeGrow(SkipListNode *nodey) {
	int grow;
	grow = rand()%2;
	if (grow)
		skipListNodeGrow(nodey);
	return grow;
}

void skipListNodeTrim(SkipListNode *nodey, int height) {
	SkipListNode **next;
	int level;

	printf("trimming node = %i to height = %i\n", nodey->data, height);

	if (nodey->height <= height)
		return;

	// Create new trimmed array of pointers
	next = malloc(sizeof(SkipListNode*) * height);
	if (next == NULL)
		return;

	// Copy existing pointers over
	for (level = 0; level < height; level++)
		next[level] = nodey->next[level];

	// Free old array
	free(nodey->next);

	// Put changes in place
	nodey->next = next;
	nodey->height = height;
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

SkipList *skipListCreate() {
	return skipListCreateH(1);
}

SkipList *skipListCreateH(int height) {
	SkipList *skippy;
	skippy = malloc(sizeof(SkipList));
	skippy->size = 0;
	skippy->head = skipListNodeCreate(height);

	if (skippy->head == NULL) {
		free(skippy);
		return NULL;
	}

	return skippy;
}

SkipList *skipListDestroy(SkipList *skippy) {
	SkipListNode *nodey, *nextNode;

	if (skippy == NULL)
		return NULL;

	nodey = skippy->head;
	while (nodey != NULL) {
		nextNode = nodey->next[0];
		skipListNodeDestroy(nodey);
		nodey = nextNode;
	}
	free(skippy);

	return NULL;
}

int skipListHeight(SkipList *skippy) {
	if (skippy == NULL)
		return -1;

	if (skippy->head == NULL)
		return -1;

	return skippy->head->height;
}

int generateRandomHeight(int maxHeight) {
	int h = 1;
	while (rand()%2 == 0 && h < maxHeight) {
		h++;
	}
	return h;
}

void skipListInsert(SkipList *skippy, int data) {
	skipListInsertH(skippy, data, generateRandomHeight(skipListHeight(skippy)));
}

void skipListInsertH(SkipList *skippy, int data, int height) {
	SkipListNode *newNode, *node;
	int level;

	if (skippy == NULL)
		return;

	// Increment size and check if we need to grow the skip list
	skippy->size++;
	if (getMaxHeight(skippy->size) > skippy->head->height)
		skipListGrow(skippy);

	// Create the new node
	newNode = skipListNodeCreate(height);
	if (newNode == NULL)
		return;

	newNode->data = data;

	level = skipListHeight(skippy) - 1;
	node = skippy->head;
	while (level >= 0) {
		if (skipListNodeGetNext(node, level) != NULL &&
		    skipListNodeGetNext(node, level)->data < data) {
			// Skip forward
			node = skipListNodeGetNext(node, level);
		} else {
			if (level < height) {
				// Linked list insertion
				skipListNodeSetNext(newNode, level, skipListNodeGetNext(node, level));
				skipListNodeSetNext(node, level, newNode);
			}
			// Drop down a level
			level--;
		}
	}
}

int skipListContains(SkipList *skippy, int data) {
	SkipListNode *nodey;
	int level = skippy->head->height;

	nodey = skippy->head;
	while (level >= 0) {
		if (nodey->next[level] != NULL) {
			if (nodey->next[level]->data == data) {
				break;
			} else if (nodey->next[level]->data < data) {
				nodey = nodey->next[level];
			} else {
				level--;
			}
		} else {
			level--;
		}
	}

	return level != -1;
}

void skipListDelete(SkipList *skippy, int data) {
	SkipListNode *nodey, *killNode = NULL;
	int level = skippy->head->height - 1;

	nodey = skippy->head;
	while (level >= 0) {
		if (nodey->next[level] != NULL) {
			if (nodey->next[level]->data == data) {
				killNode = nodey->next[level];
				nodey->next[level] = nodey->next[level]->next[level];
				level--;
			} else if (nodey->next[level]->data < data) {
				nodey = nodey->next[level];
			} else {
				level--;
			}
		} else {
			level--;
		}
	}

	if (killNode != NULL) {
		skipListNodeDestroy(killNode);
		skippy->size--;
	}

	while (skippy->head->height > getMaxHeight(skippy->size))
		skipListTrim(skippy);
}

void skipListGrow(SkipList *skippy) {
	SkipListNode *nodey, *prevNode;
	int level;

	level = skippy->head->height;
	skipListNodeGrow(skippy->head);

	nodey = skippy->head;
	prevNode = skippy->head;

	while (nodey->next[level] != NULL) {
		if (skipListNodeMaybeGrow(nodey->next[level])) {
			prevNode->next[level] = nodey;
			prevNode = nodey;	
		}
		nodey = nodey->next[level];
	}	
}

void skipListTrim(SkipList *skippy) {
	SkipListNode *nodey, *nextNode;
	int level = skippy->head->height - 1;
	nodey = skippy->head;
	while (nodey != NULL) {
		nextNode = nodey->next[level];
		skipListNodeTrim(nodey, level);
		nodey = nextNode;
	}
}

void skipListPrint(SkipList *skippy) {
	int level;
	SkipListNode *node;
	if (skippy == NULL) {
		printf("(null SkipList)\n");
		return;
	}

	printf("SkipList of size = %i\n", skippy->size);
	node = skippy->head;
	while (node != NULL) {
		printf("%i\t", node->data);
		for (level = 0; level < node->height; level++) {
			if (node->next[level] != NULL) {
				printf("%i\t", node->next[level]->data);
			} else {
				printf("X\t");
			}
		}
		printf("\n");
		node = node->next[0];
	}
}

int getMaxHeight(int size) {
	if (size == 0)
		return 1;

	return 1 + log(size) / log(2);
}

