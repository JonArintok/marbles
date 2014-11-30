
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <SDL2/SDL.h>


#define _shouldNotBeHere_ \
	printf("\n!! Should not be here: line %d of %s !!\n", __LINE__, __FILE__);

#define defaultFrameRate     30
#define defaultWindowWidth  640
#define defaultWindowHeight 480
#define tao 6.283185307179586476925286766559005768394338798750

int  curFrame = -1;
bool audioEnabled = false;
bool videoEnabled = false;
int  activeFrameform = 0;
int  exitFrameform;

#include "01_foundation.h"
#include "02_allocation.h"
#include "03_stdNodeTable.h"
#include "04_timeKeeping.h"
#include "05_parse.h"
#include "06_initialize.h"


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
		puts("initializing...");
		initializeNodes();
		if (videoEnabled)
			initializeVideo();
		
		puts("running...");
		//printf("frameRate: %f\n", frameRate);//uncomment this when using valgrind
		
		long frameTimeStamp = getMicroseconds();
		int csn = frameforms[activeFrameform].curStateNode;
		nodeIndex nextRoot = frameforms[activeFrameform].nextFrameform;
		nodeIndex videoRoot = frameforms[activeFrameform].videoOut;
		outType   videoOut;
		//nodeIndex audioRoot = frameforms[activeFrameform].audioOut;
		bool running = true;
		
		//                     //
		//   T H E   L O O P   //
		//                     //
		while (running) {
			curFrame++;
			
			//check events
			if (videoEnabled) {
				SDL_Event event;
				while (SDL_PollEvent(&event) != 0) {
					if (event.type == SDL_QUIT)
						running = false;
				}
			}
			
			//evaluate the bodies, results written to .hotState
			for (int i = 0; i <= csn; i++) {
				nodeIndex n = frameforms[activeFrameform].stateNodes[i] + 1;
				frameforms[activeFrameform].hotState[i] = 
					_output_(n, nullFnCallArgs)
			}
			
			//copy the hotState to the stateNodes cache
			for (int i = 0; i <= csn; i++) {
				nodeIndex n = frameforms[activeFrameform].stateNodes[i];
				outType hotState = frameforms[activeFrameform].hotState[i];
				if (isAnArray(n)) {
					size_t newDataSize = hotState.B.dataSize;
					nodes[n].cache.B.dimenX = hotState.B.dimenX;
					nodes[n].cache.B.dimenY = hotState.B.dimenY;
					nodes[n].cache.B.dimenZ = hotState.B.dimenZ;
					if (!nodes[n].cache.B.data) {
						printf("mallocing data from the loop, node %i\n", n);
						nodes[n].cache.B.data = malloc(newDataSize);
						addLoadedNode(n);
					}
					else if (nodes[n].cache.B.dataSize < newDataSize) {
						printf("reallocing data from the loop, node %i\n", n);
						nodes[n].cache.B.data = 
							realloc(nodes[n].cache.B.data, newDataSize);
					}
					nodes[n].cache.B.dataSize = newDataSize;
					memcpy(nodes[n].cache.B.data, hotState.B.data, newDataSize);
				}
				else
					nodes[n].cache = hotState;
				if (!videoEnabled)
					printf("%d:\t%f\n", i, nodes[n].cache.n);
			}
			if (!videoEnabled)
				puts("");
			
			
			//present video data
			if (videoRoot <= curNode) {
				videoOut = _output_(videoRoot+1, nullFnCallArgs)
				if (
					videoOut.B.dimenX < videoWidth ||
					videoOut.B.dimenY < videoHeight
				) {
					//??... for now:
					_shouldNotBeHere_
					printf("videoOut.B.dimenX.. %i\n", (int)videoOut.B.dimenX);
					printf("videoOut.B.dimenY.. %i\n", (int)videoOut.B.dimenY);
					printf("videoWidth......... %i\n", videoWidth);
					printf("videoHeight........ %i\n", videoHeight);
					printf("windowWidth........ %i\n", windowWidth);
					printf("windowHeight....... %i\n", windowHeight);
				}
				SDL_UpdateTexture(
					texture,
					NULL, 
					(uint32_t*)videoOut.B.data,
					videoWidth * sizeof(uint32_t)
				);
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);
			}
			
			
			//next frameform is determined between frames
			if (nextRoot <= curNode) {
				outType nextRootOut = _output_(nextRoot+1, nullFnCallArgs)
				activeFrameform = nextRootOut.n;
				if (activeFrameform > curFrameform)
					break;
				csn = frameforms[activeFrameform].curStateNode;
				nextRoot = frameforms[activeFrameform].nextFrameform;
				videoRoot = frameforms[activeFrameform].videoOut;
				//audioRoot = frameforms[activeFrameform].audioOut;
			}
			
			frameWait(&frameTimeStamp);
		}
	}
	
	
	puts("exiting...");
	if (videoEnabled) {
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
	cleanUp();
	
	puts("finished.");
	return 0;
}

