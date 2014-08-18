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
	
	
	init_Allocation();
	parse();
	fclose(fileStream);
	
	int  exitFrameform   = curFrameform + 1;
	int  activeFrameform = 0;
	
	
	if (!errorCount) {
		//initialize global variables, numLits evaluated immediately
		for (int i = 0; i <= gCurRootNode; i++) {
			if (nodes[ rootNodes[i] ].evaluate == eval_varDef) {
				if (nodes[ rootNodes[i+1] ].evaluate == eval_numLit)
					_evaluateNode_(rootNodes[i])
				else
					_evaluateNode_(rootNodes[i]+1)
			}
		}
		//evaluate all global variables
		for (int i = 0; i <= gCurRootNode; i++) {
			if (nodes[ rootNodes[i] ].evaluate == eval_varDef)
				_evaluateNode_(rootNodes[i])
		}
		//initialize variables in every frameform, numLits evaluated immediately
		for (int ffi = 0; ffi <= curFrameform; ffi++) {
			for (int rni = 0; rni <= frameforms[ffi].curRootNode; rni++) {
				nodeIndex n = frameforms[ffi].rootNodes[rni];
				if (nodes[n].evaluate == eval_varDef) {
					if (nodes[n+1].evaluate == eval_numLit)
						_evaluateNode_(n)
					else
						_evaluateNode_(n+1)
				}
			}
		}
		//evaluate variables in every frameform
		for (int ffi = 0; ffi <= curFrameform; ffi++) {
			for (int rni = 0; rni <= frameforms[ffi].curRootNode; rni++) {
				nodeIndex n = frameforms[ffi].rootNodes[rni];
				if (nodes[n].evaluate == eval_varDef)
					_evaluateNode_(rootNodes[i])
			}
		}
		
		
		
		
		
		
		//timeFrame may eventually be set in the loop 
		//to accommodate variable framerates
		long  timeFrame = 1e6/run_fps;
		long  timeA = getMicroseconds();
		long  timeB;
		
		//the loop
		while (activeFrameform != exitFrameform) {
			curFrame++;
			nodeIndex n;
			//evaluate the bodies
			for (
				int i = 0;
				i <= frameforms[activeFrameform].curStateNode;
				i++
			) {
				n = frameforms[activeFrameform].stateNodes[i] + 1;
				_evaluateNode_(n)
			}
			//update the state and print it
			for (
				int i = 0;
				i <= frameforms[activeFrameform].curStateNode;
				i++
			) {
				nodeIndex n = frameforms[activeFrameform].stateNodes[i];
				_evaluateNode_(n)
				printf("%d:\t%f\n", i, nodes[n].output.n);
			}
			
			//temporary until conditionals are implemented
			if (curFrame == 7)
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

