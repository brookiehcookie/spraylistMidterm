#include <stdio.h>
#include <stdlib.h>

#include "SkipList.h"

int main(int argc, char **argv) {
	int size;
	/*if (argc < 2) {
		fprintf(stderr, "Ooops\n");
		exit(1);
	}*/

	//size = atoi(argv[1]);
	for (size = 0; size < 16; size++)
		printf("The max height of a skip list with %i nodes is %i.\n", size, getMaxHeight(size));

	return 0;
}