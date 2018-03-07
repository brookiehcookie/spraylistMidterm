#include <stdio.h>
#include <stdlib.h>

#include "SkipList.h"

SkipList *skippy = NULL;

void init() {
	skippy = skipListDestroy(skippy);
	skippy = skipListCreate();
	if (skippy == NULL) {
		fprintf(stderr, "Fail whale :(\n");
		exit(1);
	}
}

int main(int argv, char **argc) {
	SkipListNode *nodey;
	FILE *fp;
	char c;
	char buffer[64];
	int i, j;

	init();

	do {
		printf("> ");
		scanf("%s", buffer);
		if (buffer[0] == '+') {
			scanf("%i", &i);
			skipListInsert(skippy, i);
			printf("Inserted value %i\n", i);
		} else if (buffer[0] == 'p') {
			skipListPrint(skippy);
		} else if (buffer[0] == '-') {
			scanf("%i", &i);
			j = skippy->size;
			skipListDelete(skippy, i);
			if (skippy->size == j)
				printf("Deleted value %i\n", i);
			else
				printf("Value %i does not exist\n", i);
		} else if (buffer[0] == '?') {
			scanf("%i", &i);
			if (skipListContains(skippy, i)) {
				printf("Skip list CONTAINS %i\n", i);
			} else {
				printf("Skip list does NOT contain %i\n", i);
			}
		} else if (buffer[0] == 'c') {
			printf("Cleared skip list\n");
			init();
		} else if (buffer[0] == 'l') {
			scanf("%s", buffer);
			fp = fopen(buffer, "r");
			if (fp != NULL) {
				fscanf(fp, "%i", &i);
				for (; i > 0; i--) {
					fscanf(fp, "%i", &j);
					skipListInsert(skippy, j);
				}
				printf("Loaded skip list from file %s\n", buffer);;
				fclose(fp);
			} else {
				printf("Could not find file %s\n", buffer);
			}
		} else if (buffer[0] == 's') {
			scanf("%s", buffer);
			fp = fopen(buffer, "w");
			if (fp != NULL) {
				fprintf(fp, "%i", skippy->size);
				nodey = skippy->head->next[0];
				while (nodey != NULL) {
					fprintf(fp, " %i", nodey->data);
					nodey = nodey->next[0];
				}
				fclose(fp);
				printf("Saved skip list from file %s\n", buffer);
			} else {
				printf("Could not open file %s\n", buffer);
			}
		} else if (buffer[0] == 'q') {
			printf("Quitting\n");
		}
	} while (c != 'q');

	return 0;
}