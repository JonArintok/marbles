
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

outType nullFnCallArgs[maxChildren] = {};

void initialize(void) {
	//assign all global literal variables
	for (int i = 0; i <= gCurRootNode; i++) {
		nodeIndex n = gRootNodes[i];
		if (
			nodes[n].evaluate == eval_varDef &&
			nodes[n+1].evaluate == eval_numLit
		) {
			_output_(n, nullFnCallArgs)
		}
	}
	//assign all literal variables in every frameform
	for (int ffi = 0; ffi <= curFrameform; ffi++) {
		for (int rni = 0; rni <= frameforms[ffi].curRootNode; rni++) {
			nodeIndex n = frameforms[ffi].rootNodes[rni];
			if (
				nodes[n].evaluate == eval_varDef &&
				nodes[n+1].evaluate == eval_numLit
			) {
				_output_(n, nullFnCallArgs)
			}
		}
	}
	
	//global outputs
	if (frameRateRoot != maxNodeIndex) {
		outType frro = _output_(frameRateRoot, nullFnCallArgs)
		frameRate = frro.n;
	}
	
	
	//evaluate nonliteral global variables
	for (int i = 0; i <= gCurRootNode; i++) {
		nodeIndex n = gRootNodes[i];
		if (
			nodes[n].evaluate == eval_varDef &&
			nodes[n+1].evaluate != eval_numLit
		) {
			nodes[n+1].cache = _output_(n+1, nullFnCallArgs)
		}
	}
	//evaluate nonliteral variables in every frameform
	for (int ffi = 0; ffi <= curFrameform; ffi++) {
		for (int rni = 0; rni <= frameforms[ffi].curRootNode; rni++) {
			nodeIndex n = frameforms[ffi].rootNodes[rni];
			if (
				nodes[n].evaluate == eval_varDef &&
				nodes[n+1].evaluate != eval_numLit
			) {
				nodes[n+1].cache = _output_(n+1, nullFnCallArgs)
			}
		}
	}
	//assign nonliteral global variables
	for (int i = 0; i <= gCurRootNode; i++) {
		nodeIndex n = gRootNodes[i];
		if (
			nodes[n].evaluate == eval_varDef &&
			nodes[n+1].evaluate != eval_numLit
		) {
			_output_(n, nullFnCallArgs)
		}
	}
	//assign nonliteral variables in every frameform
	for (int ffi = 0; ffi <= curFrameform; ffi++) {
		for (int rni = 0; rni <= frameforms[ffi].curRootNode; rni++) {
			nodeIndex n = frameforms[ffi].rootNodes[rni];
			if (
				nodes[n].evaluate == eval_varDef &&
				nodes[n+1].evaluate != eval_numLit
			) {
				_output_(n, nullFnCallArgs)
			}
		}
	}
}



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
	
	puts("reading...");
	
	init_Allocation();
	parse();
	fclose(fileStream);
	
	if (!errorCount) {
		initialize();
		
		puts("running...");
		printf("frameRate: %f\n", frameRate);//uncomment this when using valgrind
		
		long frameTimeStamp = getMicroseconds();
		//the loop
		while (true) {
			curFrame++;
			int csn = frameforms[activeFrameform].curStateNode;
			nodeIndex nextRoot = frameforms[activeFrameform].nextFrameform;
			
			//evaluate the bodies
			for (int i = 0; i <= csn; i++) {
				nodeIndex n = frameforms[activeFrameform].stateNodes[i] + 1;
				nodes[n].cache = _output_(n, nullFnCallArgs)
			}
			
			//update the state and print it (for now)
			for (int i = 0; i <= csn; i++) {
				nodeIndex n = frameforms[activeFrameform].stateNodes[i];
				_output_(n, nullFnCallArgs)
				printf("%d:\t%f\n", i, nodes[n].cache.n);
			}
			puts("");
			
			//next frameform is determined between frames
			if (nextRoot > -1) {
				outType nextRootOut = _output_(nextRoot, nullFnCallArgs)
				activeFrameform = nextRootOut.n;
				if (activeFrameform == exitFrameform)
					break;
			}
			
			frameWait(&frameTimeStamp);
		}
	}
	
	cleanUp();
	return 0;
}

