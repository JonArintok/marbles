
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
//outType   audioOut;
bool running = false;
long frameTimeStamp;

int threadCount;
SDL_atomic_t stateThreadPunch;
SDL_atomic_t videoPunch;

#define _threadWait_ microSleep(threadCount*10);


//this function needs to be called from the main thread
void renderVideoOut(void) {
	if (videoRoot <= curNode) {
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
		if (activeFrameform > curFrameform) {
			running = false;
		}
		csn       = frameforms[activeFrameform].curStateNode;
		nextRoot  = frameforms[activeFrameform].nextFrameform;
		videoRoot = frameforms[activeFrameform].videoOut;
		//audioRoot = frameforms[activeFrameform].audioOut;
	}
	
	if (videoEnabled) {
		SDL_AtomicIncRef(&videoPunch);// from 0 to 1
		while (SDL_AtomicGet(&videoPunch)) {
			_threadWait_
		}
		
		//frameWait(&frameTimeStamp);
		
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT)
				running = false;
		}
	}
	
	curFrame++;
}


#define _stub_ //printf("TI:%i   line:%3i   STP:%2i\n", threadIndex, __LINE__, SDL_AtomicGet(&stateThreadPunch));

int stateThread(void *ti) {
	const int threadIndex = *(int*)ti;
	while (true) {
		
		//evaluate the bodies, results written to .hotState
		for (int i = threadIndex; i <= csn; i += threadCount) {
			const nodeIndex n = frameforms[activeFrameform].stateNodes[i] + 1;
			frameforms[activeFrameform].hotState[i] = 
				output(n, maxNodeIndex, nullFnCallArgs);
		}
		
		SDL_AtomicIncRef(&stateThreadPunch);
		while (SDL_AtomicGet(&stateThreadPunch) < threadCount) {
			_stub_
			_threadWait_
		}
		
		//copy the hotState to the stateNodes cache
		for (int i = threadIndex; i <= csn; i += threadCount) {
			const nodeIndex n = frameforms[activeFrameform].stateNodes[i];
			const outType  hs = frameforms[activeFrameform].hotState[i];
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
		while (SDL_AtomicGet(&stateThreadPunch) < threadCount*2) {
			_stub_
			_threadWait_
		}
		
		if (!threadIndex) {
			if (videoEnabled) {
				videoOut = output(videoRoot+1, maxNodeIndex, nullFnCallArgs);
			}
			else {
				// this is temporary
				for (int i = 0; i <= csn; i++) {
					nodeIndex n = frameforms[activeFrameform].stateNodes[i];
					printf("%d:\t%f\n", i, nodes[n].cache.n);
				}
				puts("");
			}
		}
		
		SDL_AtomicIncRef(&stateThreadPunch);
		if (!threadIndex) {
			_stub_
			while (SDL_AtomicGet(&stateThreadPunch) < threadCount*3) {
				_threadWait_
			}
			betweenFrames();
			SDL_AtomicSet(&stateThreadPunch, 0);
		}
		if (threadIndex) {
			_stub_
			while (SDL_AtomicGet(&stateThreadPunch) > threadCount) {
				_threadWait_
			}
		}
		
		if (!running) {
			return 0;
		}
	}
}



int main(int argc, char **argv) {
	fileName = argv[1];
	if (!fileName) {
		puts("usage: marblerun hello.mrbl");
		return 1;
	}
	fileStream = fopen(fileName, "r");
	if (!fileStream) {
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
		SDL_AtomicSet(&videoPunch, 0);

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
		
		if (videoEnabled) {
			while (running) {
				if (SDL_AtomicGet(&videoPunch)) {
					renderVideoOut();
					SDL_AtomicDecRef(&videoPunch);// from 1 to 0
				}
				_threadWait_
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

