
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>


#define _shouldNotBeHere_ \
	printf("\n!! Should not be here: line %d of %s !!\n", __LINE__, __FILE__);

int  curFrame = -1;
bool audioEnabled = false;
bool videoEnabled = false;
int  activeFrameform = 0;
int  exitFrameform;

#define defaultFrameRate    30
#define defaultWindowWidth  640
#define defaultWindowHeight 480

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
	
	
	
	
	
	if (!errorCount) {
		//assign all global literal variables
		for (int i = 0; i <= gCurRootNode; i++) {
			nodeIndex n = gRootNodes[i];
			if (
				(
					nodes[n].evaluate == eval_varDef || 
					nodes[n].evaluate == eval_outDef
				) &&
				nodes[n+1].evaluate == eval_numLit
			) {
				_evaluateNode_(n)
			}
		}
		//assign all literal variables in every frameform
		for (int ffi = 0; ffi <= curFrameform; ffi++) {
			for (int rni = 0; rni <= frameforms[ffi].curRootNode; rni++) {
				nodeIndex n = frameforms[ffi].rootNodes[rni];
				if (
					(
						nodes[n].evaluate == eval_varDef || 
						nodes[n].evaluate == eval_outDef
					) &&
					nodes[n+1].evaluate == eval_numLit
				) {
					_evaluateNode_(n)
				}
			}
		}
		//evaluate nonliteral global variables
		for (int i = 0; i <= gCurRootNode; i++) {
			nodeIndex n = gRootNodes[i];
			if (
				(
					nodes[n].evaluate == eval_varDef || 
					nodes[n].evaluate == eval_outDef
				) &&
				nodes[n+1].evaluate != eval_numLit
			) {
				_evaluateNode_(n+1)
			}
		}
		//evaluate nonliteral variables in every frameform
		for (int ffi = 0; ffi <= curFrameform; ffi++) {
			for (int rni = 0; rni <= frameforms[ffi].curRootNode; rni++) {
				nodeIndex n = frameforms[ffi].rootNodes[rni];
				if (
					(
						nodes[n].evaluate == eval_varDef || 
						nodes[n].evaluate == eval_outDef
					) &&
					nodes[n+1].evaluate != eval_numLit
				) {
					_evaluateNode_(n+1)
				}
			}
		}
		//assign nonliteral global variables
		for (int i = 0; i <= gCurRootNode; i++) {
			nodeIndex n = gRootNodes[i];
			if (
				(
					nodes[n].evaluate == eval_varDef || 
					nodes[n].evaluate == eval_outDef
				) &&
				nodes[n+1].evaluate != eval_numLit
			) {
				_evaluateNode_(n)
			}
		}
		//assign nonliteral variables in every frameform
		for (int ffi = 0; ffi <= curFrameform; ffi++) {
			for (int rni = 0; rni <= frameforms[ffi].curRootNode; rni++) {
				nodeIndex n = frameforms[ffi].rootNodes[rni];
				if (
					(
						nodes[n].evaluate == eval_varDef || 
						nodes[n].evaluate == eval_outDef
					) &&
					nodes[n+1].evaluate != eval_numLit
				) {
					_evaluateNode_(n)
				}
			}
		}
		
		if (frameRateRoot > -1)
			frameRate = nodes[frameRateRoot].output.n;
		
		long frameTimeStamp = getMicroseconds();
		//the loop
		while (activeFrameform != exitFrameform) {
			curFrame++;
			int csn = frameforms[activeFrameform].curStateNode;
			nodeIndex nfRoot = frameforms[activeFrameform].nextFrameform;
			
			//evaluate the bodies
			for (int i = 0; i <= csn; i++) {
				nodeIndex n = frameforms[activeFrameform].stateNodes[i] + 1;
				_evaluateNode_(n)
			}
			//evaluate next frameform
			if (nfRoot > -1)
				_evaluateNode_(nfRoot+1)
			
			
			//update the state and print it (for now)
			for (int i = 0; i <= csn; i++) {
				nodeIndex n = frameforms[activeFrameform].stateNodes[i];
				_evaluateNode_(n)
				printf("%d:\t%f\n", i, nodes[n].output.n);
			}
			if (nfRoot > -1) {
				_evaluateNode_(nfRoot)
				activeFrameform = nodes[nfRoot].output.n;
			}
			
			frameWait(&frameTimeStamp);
		}
	}
	
	cleanUp();
	return 0;
}

