
int csn;
nodeIndex nextRoot;
nodeIndex videoRoot;
outType   videoOut;
//nodeIndex audioRoot;
//outType   audioOut;
bool running = false;
long frameTimeStamp;

SDL_atomic_t stateThreadPunch;
SDL_atomic_t videoPunch;


void betweenFrames(void) {
	if (nextRoot <= curNode) {
		outType nextRootOut = output(nextRoot+1, -1, nullFnCallArgs);
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
	
	if (!threadIndex) {
		puts("initializing...");
		initializeNodes();
		if (videoEnabled) {
			SDL_AtomicIncRef(&videoPunch);// from 0 to 1
			while (SDL_AtomicGet(&videoPunch)) {
				_threadWait_
			}
		}
		puts("running...");
		SDL_AtomicSet(&stateThreadPunch, 0);
	}
	else {
		_stub_
		while (SDL_AtomicGet(&stateThreadPunch) > threadCount) {
			if (doATask()) {
				_threadWait_
			}
		}
	}
	
	while (running) {
		
		//evaluate the bodies, results written to .hotState
		for (int i = threadIndex; i <= csn; i += threadCount) {
			const nodeIndex n = frameforms[activeFrameform].stateNodes[i] + 1;
			frameforms[activeFrameform].hotState[i] = 
				output(n, -1, nullFnCallArgs);
		}
		
		SDL_AtomicIncRef(&stateThreadPunch);
		_stub_
		while (SDL_AtomicGet(&stateThreadPunch) < threadCount) {
			if (doATask()) {
				_threadWait_
			}
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
		_stub_
		while (SDL_AtomicGet(&stateThreadPunch) < threadCount*2) {
			_threadWait_
		}
		
		if (!threadIndex) {
			if (videoEnabled) {
				videoOut = output(videoRoot+1, -1, nullFnCallArgs);
			}/*
			else {
				// this is temporary
				for (int i = 0; i <= csn; i++) {
					nodeIndex n = frameforms[activeFrameform].stateNodes[i];
					printf("%d:\t%f\n", i, nodes[n].cache.n);
				}
				puts("");
			}*/
		}
		
		SDL_AtomicIncRef(&stateThreadPunch);
		if (!threadIndex) {
			_stub_
			while (SDL_AtomicGet(&stateThreadPunch) < threadCount*3) {
				if (doATask()) {
					_threadWait_
				}
			}
			betweenFrames();
			SDL_AtomicSet(&stateThreadPunch, 0);
		}
		else {
			_stub_
			while (SDL_AtomicGet(&stateThreadPunch) > threadCount) {
				if (doATask()) {
					_threadWait_
				}
			}
		}
	}
	return 0;
}

//this function needs to be called from the main thread
void renderVideoOut(void) {
	if (videoRoot > curNode)
		_shouldNotBeHere_
	if (videoOut.B.dimenX < videoWidth || videoOut.B.dimenY < videoHeight)
		_shouldNotBeHere_
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
