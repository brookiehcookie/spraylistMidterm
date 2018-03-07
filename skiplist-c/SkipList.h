#ifndef __SKIPLIST_H
#define __SKIPLIST_H

// SkipListNode 

typedef struct SkipListNode {
	int data;
	int height;
	struct SkipListNode **next;
} SkipListNode;

SkipListNode *skipListNodeCreate();
SkipListNode *skipListNodeDestroy(SkipListNode *nodey);
SkipListNode *skipListNodeGetNext(SkipListNode *nodey, int level);
void skipListNodeSetNext(SkipListNode *nodey, int level, SkipListNode *newNext);
void skipListNodeTrim(SkipListNode *nodey, int height);

// SkipList functions

typedef struct SkipList {
	int size;
	SkipListNode *head;
} SkipList;

SkipList *skipListCreate();
SkipList *skipListCreateH(int height);
SkipList *skipListDestroy(SkipList *skippy);
int skipListHeight(SkipList *skippy);
void skipListInsert(SkipList *skippy, int data);
void skipListInsertH(SkipList *skippy, int data, int height);
int skipListContains(SkipList *skippy, int data);
void skipListDelete(SkipList *skippy, int data);
void skipListGrow(SkipList *skippy);
void skipListTrim(SkipList *skippy);
void skipListPrint(SkipList *skippy);

#endif