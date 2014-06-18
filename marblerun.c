//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "foundation.h"
#include "stdNodeTable.h"
#include "parse.h"



//void cleanUp(void) {
//	
//}


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
	
	//initialize nodes
	for (int i = 0; i<nodePageSize; i++) {
		nodes[i].name = "empty";
	}
	
	
	
	while (true) {
		
		rootNodes[currentRootNode] = currentNode;
		
		getNode();
		if (!noErrors || reachedEOF)
			break;
		
		//if the current rootNode is still "empty", then ignore it,
		//otherwise, we count the root node to be evaluated
		if (strcmp( nodes[rootNodes[currentRootNode]].name, "empty" )) {
			currentRootNode++;
			// reallocate the rootNodes to a larger array if necessary
		}
		
	}
	
	fclose(fileStream);
	
	if (noErrors) {
		puts("results:");
		
		for (int i = 0; i<currentRootNode; i++) {
			nodes[ rootNodes[i] ].evaluate( rootNodes[i] );
			printf("  %2d: %9.3f\n", i, nodes[rootNodes[i]].output.n);
		}
	}
	
	//cleanUp();
	return 0;
}

