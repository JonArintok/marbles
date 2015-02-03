
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <SDL2/SDL.h>

#ifdef __linux__
	#include <time.h>
	#define microSleep usleep
#elif __APPLE__
#elif _WIN32
#endif


#define _shouldNotBeHere_ \
	printf("\n!! Should not be here: line %d of %s !!\n\n", __LINE__, __FILE__);

#define defaultWindowWidth  640
#define defaultWindowHeight 480
#define tao 6.283185307179586476925286766559005768394338798750

bool audioEnabled = false;
bool videoEnabled = false;
int curFrame = 0;
int activeFrameform = 0;
int exitFrameform;

SDL_Window   *window   = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture  *texture  = NULL;

int threadCount;
#define _threadWait_ microSleep(threadCount*12);


#include "01_foundation.h"
#include "02_allocation.h"
#include "03_stdNodeTable.h"
#include "04_timeKeeping.h"
#include "05_parse.h"
#include "06_initialize.h"
#include "07_run.h"



int main(int argc, char **argv) {
	fileName = argv[1];
	if (!fileName) {
		puts("usage: marblerun hello.mrbl");
		return 1;
	}
	fileStream = fopen(fileName, "r");
	if (!fileStream) {
		printf("could not open '%s'\n", fileName);
		return 2;
	}
	
	
	atexit(calledAtExit);
	init_Allocation();
	puts("reading...");
	parse();
	fclose(fileStream);
	
	if (errorCount)
		exit(3);
	
	csn       = frameforms[activeFrameform].curStateNode;
	nextRoot  = frameforms[activeFrameform].nextFrameform;
	videoRoot = frameforms[activeFrameform].videoOut;
	//audioRoot = frameforms[activeFrameform].audioOut;
	SDL_AtomicSet(&stateThreadPunch, 10000);
	SDL_AtomicSet(&videoPunch, 0);

	threadCount = SDL_GetCPUCount();
	printf("using %i threads\n", threadCount);

	running = true;
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
		while (!SDL_AtomicGet(&videoPunch))
			_threadWait_
		initializeVideo();
		SDL_AtomicDecRef(&videoPunch);// from 1 to 0
		while (running) {
			if (SDL_AtomicGet(&videoPunch)) {
				renderVideoOut();
				SDL_AtomicDecRef(&videoPunch);// from 1 to 0
			}
			_threadWait_
		}
		SDL_AtomicSet(&videoPunch, 0);// other threads aren't waiting
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	}
	
	for (int i = 0; i < threadCount; i++)
		SDL_WaitThread(threads[i], NULL);
	
	puts("exiting...");
	SDL_Quit();
	exit(0);
}
