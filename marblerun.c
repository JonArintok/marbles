//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "foundation.h"
#include "arithmetic.h"
#include "stdNodeTable.h"
#include "parse.h"


node  rootNodes[rootNodePageSize];
uint  rootNodeCount = 0;


void freeArgs(node *toBeFreed) {
	if (toBeFreed->arity) {
		uint i = 0;
		for (; i < toBeFreed->arity; i++)
			freeArgs(&toBeFreed->arguments[i]);
		free(toBeFreed->arguments);
	}
}

void cleanUp(void) {
	uint i = 0;
	for (; i<rootNodeCount; i++)
		freeArgs(&rootNodes[i]);
}


int main(int argc, char **argv) {
	char *fileName = argv[1];
	if (fileName == NULL) {
		puts("usage: marblerun <file name>");
		return 1;
	}
	fileStream = fopen(fileName, "r");
	if (fileStream == NULL) {
		printf("error: could not open '%s'\n\n", fileName);
		return 2;
	}
	
	buildStdNodeTable();
	//'numLit' is used to initialize the rootNode
	rootNodes[0] = node_numLit;
	
	while (1) {
		
		getNode(&rootNodes[rootNodeCount]);
		if (!noErrors || reachedEOF)
			break;
		//if the current rootNode is still 'numLit', then ignore it,
		//otherwise, we count the root node to be evaluated
		if ( strcmp(rootNodes[rootNodeCount].name, "numLit") ) {
			rootNodeCount++;
			//initialize next rootNode
			rootNodes[rootNodeCount] = node_numLit;
		}
		
	}
	
	if (noErrors) {
		puts("results:");
		uint i = 0;
		for (; i<rootNodeCount; i++) {
			rootNodes[i].evaluate(&rootNodes[i]);
			printf("  %2d: %9.3f\n", i, rootNodes[i].output.n);
		}
	}
	
	fclose(fileStream);
	cleanUp();
	return 0;
}




