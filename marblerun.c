//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "foundation.h"
#include "stdNodeTable.h"
#include "parse.h"



void cleanUp(void) {
	for (int i = 0; i < currentNode; i++)
		free( nodes[i].name );
	free(nodes);
	free(rootNodes);
	for (int i = 0; i < currentFrameform; i++)
		free( frameforms[i].stateNodes );
	free( frameforms );
}


int main(int argc, char **argv) {
	fileName = argv[1];
	if (fileName == NULL) {
		puts("usage: marblerun <file name>");
		return 1;
	}
	fileStream = fopen(fileName, "r");
	if (fileStream == NULL) {
		putError("could not open:");
		printf("\t\t%s\n", fileName);
		return 2;
	}
	
	//initialize nodes
	for (int i = 0; i<nodePageSize; i++) {
		nodes[i].name = "empty";
	}
	
	parse();
	
	fclose(fileStream);
	
	if (noErrors) {
		
		//evaluate the bodies
		for (int i = 0; i < currentFrameform.currentStateNode; i++) {
			evaluateNode( currentFrameform.stateNodes[i] + 1 );
		}
		//update the state and print it
		for (int i = 0; i < currentFrameform.currentStateNode; i++) {
			evaluateNode( currentFrameform.stateNodes[i] );
			printf(
				"%s:\t%f\n", 
				i, nodes[ currentFrameform.stateNodes[i].output.n ]
			);
		}
	}
	
	cleanUp();
	return 0;
}

