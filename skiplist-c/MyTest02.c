#include <stdio.h>
#include <stdlib.h>

#include "SkipList.h"

int main(int argv, char **argc) {
	SkipList *skippy;
	skippy = skipListCreate();
	skipListInsert(skippy, 2);
	skipListInsert(skippy, 4);
	skipListInsert(skippy, 9);
	skipListInsert(skippy, 10);
	skipListInsert(skippy, 18);
	skipListInsert(skippy, 20);
	skipListInsert(skippy, 27);
	skipListInsert(skippy, 30);
	skipListInsert(skippy, 36);
	skipListInsert(skippy, 41);
	skipListInsert(skippy, 47);
	skipListInsert(skippy, 50);
	skipListPrint(skippy);
}