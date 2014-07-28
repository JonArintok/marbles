//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "foundation.h"
#include "stdNodeTable.h"
#include "allocation.h"
#include "getMicroseconds.h"
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
	
	init_Allocation();
	parse();
	fclose(fileStream);
	
	int  exitFrameform   = currentFrameform + 1;
	int  activeFrameform = 0;
	
	
	
	if (noErrors) {
		//initialize variables
		for (int i = 0; i <= currentRootNode; i++) {
			if (nodes[ rootNodes[i] ].evaluate == eval_varDef)
				evaluateNode(rootNodes[i]);
		}
		
		//timeFrame may eventually be set in the loop 
		//to accommodate variable framerates
		long  timeFrame = 1e6/run_fps;
		long  timeA = getMicroseconds();
		long  timeB;
		
		//the loop
		while (activeFrameform != exitFrameform) {
			frameCount++;
			
			//evaluate the bodies
			for (
				int i = 0;
				i <= frameforms[activeFrameform].currentStateNode;
				i++
			) {
				evaluateNode( frameforms[activeFrameform].stateNodes[i] + 1 );
			}
			//update the state and print it
			nodeIndex stanodi;
			for (
				int i = 0;
				i <= frameforms[activeFrameform].currentStateNode;
				i++
			) {
				stanodi = frameforms[activeFrameform].stateNodes[i];
				evaluateNode(stanodi);
				printf("%d:\t%f\n", i, nodes[stanodi].output.n);
			}
			
			//temporary until conditionals are implemented
			if (frameCount == 4)
				activeFrameform = exitFrameform;
			else {
				timeB = getMicroseconds();
				long  timeWait = timeFrame - (timeB - timeA);
				if (timeWait < 0)
					printf("\n\n!! timeWait : %zu !!\n\n", timeWait);
				usleep(timeWait);
				timeA = getMicroseconds();
			}
		}
	}
	
	cleanUp();
	return 0;
}

