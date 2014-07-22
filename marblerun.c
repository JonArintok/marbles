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
	
	//free nodes[i].name unless it's from a static stdNode
	for (int i = 0; i <= currentNode; i++) {
		int j = 0;
		for (; j < stdNodeTableLength; j++) {
			if (stdNodeTable[j]->name == nodes[i].name)
				break;
		}
		if (j == stdNodeTableLength)
			free( nodes[i].name );
	}
	
	//free the stateNode arrays and then the frameForm array
	for (int i = 0; i <= currentFrameform; i++)
		free( frameforms[i].stateNodes );
	free( frameforms );
	
	//free this other stuff
	free(nodes);
	free(rootNodes);
}


int main(int argc, char **argv) {
	fileName = argv[1];
	if (fileName == NULL) {
		puts("usage: marblerun <file name>");
		return 1;
	}
	fileStream = fopen(fileName, "r");
	if (fileStream == NULL) {
		putError("could not open: ");
		printf("%s\n", fileName);
		return 2;
	}
	
	
	parse();
	
	fclose(fileStream);
	
	
	if (noErrors) {
		
		//evaluate the bodies
		for (
			int i = 0;
			i < frameforms[currentFrameform].currentStateNode;
			i++
		) {
			evaluateNode( frameforms[currentFrameform].stateNodes[i] + 1 );
		}
		//update the state and print it
		nodeIndex stanodi;
		for (
			int i = 0;
			i < frameforms[currentFrameform].currentStateNode;
			i++
		) {
			stanodi = frameforms[currentFrameform].stateNodes[i];
			evaluateNode(stanodi);
			printf("%d:\t%f\n", i, nodes[stanodi].output.n);
		}
	}
	
	cleanUp();
	return 0;
}

