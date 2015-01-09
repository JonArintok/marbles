
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

#define defaultWindowWidth  640
#define defaultWindowHeight 480
#define tao 6.283185307179586476925286766559005768394338798750

bool audioEnabled = false;
bool videoEnabled = false;
int curFrame = 0;
int activeFrameform = 0;
int exitFrameform;

#include "01_foundation.h"
#include "02_allocation.h"
#include "03_stdNodeTable.h"
#include "04_timeKeeping.h"
#include "05_parse.h"
#include "06_initialize.h"



int csn;
nodeIndex nextRoot;
nodeIndex videoRoot;
outType   videoOut;
//nodeIndex audioRoot;
bool running = false;
long frameTimeStamp;

void checkEvents(void) {
	if (videoEnabled) {
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT)
				running = false;
		}
	}
}

void renderVideoOut(void) {
	if (videoRoot <= curNode) {
		videoOut = output(videoRoot+1, maxNodeIndex, nullFnCallArgs);
		if (
			videoOut.B.dimenX < videoWidth ||
			videoOut.B.dimenY < videoHeight
		) {
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
}

void betweenFrames(void) {
	if (nextRoot <= curNode) {
		outType nextRootOut = output(nextRoot+1, maxNodeIndex, nullFnCallArgs);
		activeFrameform = nextRootOut.n;
		if (activeFrameform > curFrameform)
			running = false;
		csn       = frameforms[activeFrameform].curStateNode;
		nextRoot  = frameforms[activeFrameform].nextFrameform;
		videoRoot = frameforms[activeFrameform].videoOut;
		//audioRoot = frameforms[activeFrameform].audioOut;
	}
}


#define _stub_ printf("%i:%i\n", threadIndex, __LINE__);

int threadCount;
SDL_atomic_t stateThreadPunch;

int stateThread(void *ti) {
	int threadIndex = *(int*)ti;
	
	while (true) {
		_stub_
		
		//evaluate the bodies, results written to .hotState
		for (int i = threadIndex; i <= csn; i += threadCount) {
			nodeIndex n = frameforms[activeFrameform].stateNodes[i] + 1;
			frameforms[activeFrameform].hotState[i] = 
				output(n, maxNodeIndex, nullFnCallArgs);
		}
		
		SDL_AtomicIncRef(&stateThreadPunch);
		while (true) {
			if (SDL_AtomicGet(&stateThreadPunch) >= threadCount)
				break;
			SDL_Delay(1);
		}
		_stub_
		
		//copy the hotState to the stateNodes cache
		for (int i = threadIndex; i <= csn; i += threadCount) {
			nodeIndex n = frameforms[activeFrameform].stateNodes[i];
			outType hs = frameforms[activeFrameform].hotState[i];
			if (isAnArray(n)) {
				size_t newDataSize = hs.B.dataSize;
				nodes[n].cache.B.dimenX = hs.B.dimenX;
				nodes[n].cache.B.dimenY = hs.B.dimenY;
				nodes[n].cache.B.dimenZ = hs.B.dimenZ;
				setLoadedNode(n, newDataSize);
				memcpy(nodes[n].cache.B.data, hs.B.data, newDataSize);
			}
			else {
				nodes[n].cache = hs;
			}
		}
		
		SDL_AtomicIncRef(&stateThreadPunch);
		while (true) {
			if (SDL_AtomicGet(&stateThreadPunch) >= threadCount*2)
				break;
			SDL_Delay(1);
		}
		_stub_
		
		if (!threadIndex && !videoEnabled) {
			for (int i = 0; i <= csn; i++) {
				nodeIndex n = frameforms[activeFrameform].stateNodes[i];
				printf("%d:\t%f\n", i, nodes[n].cache.n);
			}
			puts("");
		}
		
// 		if (!threadIndex)
// 			frameWait(&frameTimeStamp);
// 		
// 		if (threadIndex == 1 || threadCount == 1)
// 			renderVideoOut();
// 		
// 		if (threadIndex == 2 || threadCount == 1 || (threadCount == 2 && threadIndex == 1))
// 			betweenFrames();
		
		_stub_
		SDL_AtomicIncRef(&stateThreadPunch);
		while (!threadIndex) {
			if (SDL_AtomicGet(&stateThreadPunch) >= threadCount*3) {
				renderVideoOut();
				betweenFrames();
				frameWait(&frameTimeStamp);
				checkEvents();
				curFrame++;
				SDL_AtomicSet(&stateThreadPunch, 0);
				break;
			}
			SDL_Delay(1);
		}
		while (threadIndex) {
			if (SDL_AtomicGet(&stateThreadPunch) == 0)
				break;
			SDL_Delay(1);
		}
		_stub_
		
		if (!running)
			return 0;
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
		puts("initializing...");
		initializeNodes();
		if (videoEnabled)
			initializeVideo();
		puts("running...");
		
		csn       = frameforms[activeFrameform].curStateNode;
		nextRoot  = frameforms[activeFrameform].nextFrameform;
		videoRoot = frameforms[activeFrameform].videoOut;
		//audioRoot = frameforms[activeFrameform].audioOut;
		running = true;
		threadCount = SDL_GetCPUCount();
		SDL_AtomicSet(&stateThreadPunch, 0);
		printf("using %i threads\n", threadCount);

		frameTimeStamp = getMicroseconds();
		
		SDL_Thread *threads[threadCount];
		int threadIndices[threadCount];
		for (int i = 0; i < threadCount; i++) {
			threadIndices[i] = i;
			threads[i] = SDL_CreateThread(stateThread, "stateThread", (void*)&threadIndices[i]);
			if (!threads[i]) {
				printf("error: could not create thread %i\n", i);
				exit(1);
			}
		}
		for (int i = 0; i < threadCount; i++) {
			SDL_WaitThread(threads[i], NULL);
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
	exit(0);
}

