
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define _shouldNotBeHere_ \
	printf("\n!! Should not be here: line %d of %s !!\n", __LINE__, __FILE__);

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
	
	
	//temporary
	int exitFrameform = curFrameform + 1;	
	int runLimit = 14;
	
	
	
	if (!errorCount) {
		//initialize global variables, numLits evaluated immediately
		for (int i = 0; i <= gCurRootNode; i++) {
			if (nodes[ gRootNodes[i] ].evaluate == eval_varDef) {
				if (nodes[ gRootNodes[i+1] ].evaluate == eval_numLit)
					_evaluateNode_(gRootNodes[i])
				else
					_evaluateNode_(gRootNodes[i]+1)
			}
		}
		//evaluate all global variables
		for (int i = 0; i <= gCurRootNode; i++) {
			if (nodes[ gRootNodes[i] ].evaluate == eval_varDef)
				_evaluateNode_(gRootNodes[i])
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
					_evaluateNode_(n)
			}
		}
		
		
		//timeFrame may eventually be set in the loop 
		//to accommodate variable framerates
		long frameTimeStamp = getMicroseconds();
		
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
			if (curFrame == runLimit)
				activeFrameform = exitFrameform;
			else
				frameWait(&frameTimeStamp);
		}
	}
	
	cleanUp();
	return 0;
}

