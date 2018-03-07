#include <stdio.h>
#include <stdlib.h>

#include "SkipList.h"

int main(int argv, char **argc) {
	SkipList *skippy;
	skippy = skipListCreateH(4);
	skipListInsertH(skippy, 2, 2);
	skipListInsertH(skippy, 4, 2);
	skipListInsertH(skippy, 9, 3);
	skipListInsertH(skippy, 10, 1);
	skipListInsertH(skippy, 18, 1);
	skipListInsertH(skippy, 20, 1);
	skipListInsertH(skippy, 27, 2);
	skipListInsertH(skippy, 30, 2);
	skipListInsertH(skippy, 36, 1);
	skipListInsertH(skippy, 41, 1);
	skipListInsertH(skippy, 47, 3);
	skipListInsertH(skippy, 50, 4);
	skipListPrint(skippy);
	skipListDelete(skippy, 9);
	skipListPrint(skippy);
}