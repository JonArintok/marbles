
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <SDL2/SDL.h>


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
#include "allocation.h"
#include "stdNodeTable.h"
#include "timeKeeping.h"
#include "parse.h"

outType nullFnCallArgs[maxChildren] = {};

void initializeNodes(void) {
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
	if (frameRateRoot < curNode) {
		outType o = _output_(frameRateRoot, nullFnCallArgs)
		frameRate = o.n;
	}
	if (windowWidthRoot < curNode) {
		outType o = _output_(windowWidthRoot, nullFnCallArgs)
		windowWidth = o.n;
	}
	if (windowHeightRoot < curNode) {
		outType o = _output_(windowHeightRoot, nullFnCallArgs)
		windowHeight = o.n;
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



SDL_Window   *window   = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture  *texture  = NULL;
outType       videoOut;

void initializeVideo() {
	SDL_Init(SDL_INIT_VIDEO);
	
	window = SDL_CreateWindow(
		"marbles",                 // window title
		SDL_WINDOWPOS_UNDEFINED,   // initial x position
		SDL_WINDOWPOS_UNDEFINED,   // initial y position
		windowWidth,               // width, in pixels
		windowHeight,              // height, in pixels
		SDL_WINDOW_OPENGL          // flags
	);
	
	if (window == NULL) {
		errorCount++;
		printf("Could not create window: %s\n", SDL_GetError());
	}
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		windowWidth, windowHeight
	);
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
		initializeNodes();
		if (videoEnabled)
			initializeVideo();
		
		puts("running...");
		printf("frameRate: %f\n", frameRate);//uncomment this when using valgrind
		
		long frameTimeStamp = getMicroseconds();
		int csn = frameforms[activeFrameform].curStateNode;
		nodeIndex nextRoot = frameforms[activeFrameform].nextFrameform;
		nodeIndex videoRoot = frameforms[activeFrameform].videoOut;
		//nodeIndex audioRoot = frameforms[activeFrameform].audioOut;
		bool running = true;
		
		//   T H E   L O O P
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
				frameforms[activeFrameform].hotState[i] = _output_(n, nullFnCallArgs)
			}
			
			//copy the hotState to the stateNodes cache
			for (int i = 0; i <= csn; i++) {
				nodeIndex n = frameforms[activeFrameform].stateNodes[i];
				nodes[n].cache = frameforms[activeFrameform].hotState[i];
				if (!videoEnabled)
					printf("%d:\t%f\n", i, nodes[n].cache.n);
			}
			if (!videoEnabled)
				puts("");
			
			
			//present video data
			if (videoRoot < curNode) {
				videoOut = _output_(videoRoot, nullFnCallArgs)
				if (
					videoOut.B.dimenX < windowWidth ||
					videoOut.B.dimenY < windowHeight
				) {
					//??... for now:
					_shouldNotBeHere_
				}
				SDL_UpdateTexture(
					texture,
					NULL, 
					(uint32_t*)videoOut.B.data,
					windowWidth * sizeof(uint32_t)
				);
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);
			}
			
			
			//next frameform is determined between frames
			if (nextRoot < curNode) {
				outType nextRootOut = _output_(nextRoot, nullFnCallArgs)
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
	
	if (videoEnabled) {
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
	cleanUp();
	return 0;
}

