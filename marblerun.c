//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "foundation.h"
#include "stdNodeTable.h"
#include "allocation.h"
#include "parse.h"


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
	
	initAllocation();
	parse();
	
	fclose(fileStream);
	
	
	if (noErrors) {
		//initialize variables
		for (int i = 0; i <= currentRootNode; i++) {
			if (nodes[ rootNodes[i] ].evaluate == eval_varDef)
				evaluateNode(rootNodes[i]);
		}
		
		
		//evaluate the bodies
		for (
			int i = 0;
			i <= frameforms[currentFrameform].currentStateNode;
			i++
		) {
			evaluateNode( frameforms[currentFrameform].stateNodes[i] + 1 );
		}
		//update the state and print it
		nodeIndex stanodi;
		for (
			int i = 0;
			i <= frameforms[currentFrameform].currentStateNode;
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

