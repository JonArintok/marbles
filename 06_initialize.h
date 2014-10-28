
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
	if (frameRateRoot <= curNode) {
		outType o = _output_(frameRateRoot, nullFnCallArgs)
		frameRate = o.n;
	}
	if (windowWidthRoot <= curNode) {
		outType o = _output_(windowWidthRoot, nullFnCallArgs)
		windowWidth = o.n;
	}
	if (windowHeightRoot <= curNode) {
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
		SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STREAMING,
		windowWidth, windowHeight
	);
}

