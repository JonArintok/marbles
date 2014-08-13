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
#include "timeKeeping.h"
#include "parse.h"




int main(int argc, char **argv) {
	fileName = argv[1];
	if (fileName == NULL) {
		puts("usage: marblerun <file name>");
		return 1;
	}
	fileStream = fopen(fileName, "r");
	if (fileStream == NULL) {
		printf("could not open: %s\n", fileName);
		return 2;
	}
	
	
	//stub
	while (!reachedEOF) {
		getLine();
		puts(&lineBuf[0]);
	}
	return 0;
	
	
	
	init_Allocation();
	parse();
	fclose(fileStream);
	
	int  exitFrameform   = currentFrameform + 1;
	int  activeFrameform = 0;
	
	
	
	if (noErrors) {
		//initialize variables, numLits get evaluated immediately
		for (int i = 0; i <= currentRootNode; i++) {
			if (nodes[ rootNodes[i] ].evaluate == eval_varDef) {
				if (nodes[ rootNodes[i+1] ].evaluate == eval_numLit)
					_evaluateNode_(rootNodes[i])
				else
					_evaluateNode_(rootNodes[i]+1)
			}
		}
		//evaluate all variables
		for (int i = 0; i <= currentRootNode; i++) {
			if (nodes[ rootNodes[i] ].evaluate == eval_varDef)
				_evaluateNode_(rootNodes[i])
		}
		
		//timeFrame may eventually be set in the loop 
		//to accommodate variable framerates
		long  timeFrame = 1e6/run_fps;
		long  timeA = getMicroseconds();
		long  timeB;
		
		//the loop
		while (activeFrameform != exitFrameform) {
			currentFrame++;
			
			//evaluate the bodies
			for (
				int i = 0;
				i <= frameforms[activeFrameform].currentStateNode;
				i++
			) {
				_evaluateNode_( frameforms[activeFrameform].stateNodes[i] + 1 )
			}
			//update the state and print it
			nodeIndex stanodi;
			for (
				int i = 0;
				i <= frameforms[activeFrameform].currentStateNode;
				i++
			) {
				stanodi = frameforms[activeFrameform].stateNodes[i];
				_evaluateNode_(stanodi)
				printf("%d:\t%f\n", i, nodes[stanodi].output.n);
			}
			
			//temporary until conditionals are implemented
			if (currentFrame == 7)
				activeFrameform = exitFrameform;
			else {
				timeB = getMicroseconds();
				long  timeWait = timeFrame - (timeB - timeA);
				if (timeWait < 0)
					printf("\n\n!! timeWait : %zu !!\n\n", timeWait);
				microSleep(timeWait);
				timeA = getMicroseconds();
			}
		}
	}
	
	cleanUp();
	return 0;
}

