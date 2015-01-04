
outType nullFnCallArgs[maxChildren] = {};


void setGlobalState(void) {
	if (frameRateRoot <= curNode)
		frameRate = nodes[frameRateRoot].cache.n;
	if (videoWidthRoot <= curNode)
		videoWidth = nodes[videoWidthRoot].cache.n;
	if (videoHeightRoot <= curNode)
		videoHeight = nodes[videoHeightRoot].cache.n;
	if (windowWidthRoot <= curNode)
		windowWidth = nodes[windowWidthRoot].cache.n;
	if (windowHeightRoot <= curNode)
		windowHeight = nodes[windowHeightRoot].cache.n;
}

void initializeNodes(void) {
	int initCount = 0;
	
	//first assign variables defined literally while incrementing initCount
	for (int i = 0; i <= curNode; i++) {
		if (nodes[i].evaluate == eval_varDef || nodes[i].evaluate == eval_gOutDef) {
			if (nodes[i+1].evaluate == eval_numLit)
				nodes[i].cache = nodes[i+1].cache;
			else
				initCount++;
		}
	}
	
	//set literally defined global outputs
	setGlobalState();
	
	//then evaluate the other variables
	outType *hotDef = malloc(sizeof(outType) * initCount);
	initCount = 0;
	for (int i = 0; i <= curNode; i++) {
		if (
			(nodes[i].evaluate == eval_varDef || nodes[i].evaluate == eval_gOutDef)
			&& nodes[i+1].evaluate != eval_numLit
		) {
			hotDef[initCount] = output(i+1, maxNodeIndex, nullFnCallArgs);
			initCount++;
		}
	}
	//then assign the other variables
	initCount = 0;
	for (int i = 0; i <= curNode; i++) {
		if (
			(nodes[i].evaluate == eval_varDef || nodes[i].evaluate == eval_gOutDef)
			&& nodes[i+1].evaluate != eval_numLit
		) {
			nodes[i].cache = hotDef[initCount];
			initCount++;
		}
	}
	free(hotDef);
	
	//set other global outputs
	setGlobalState();
}



SDL_Window   *window   = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture  *texture  = NULL;

void initializeVideo() {
	if (videoWidth < 1) {
		if (windowWidth < 1)
			videoWidth = defaultWindowWidth;
		else
			videoWidth = windowWidth;
	}
	if (videoHeight < 1) {
		if (windowHeight < 1)
			videoHeight = defaultWindowHeight;
		else
			videoHeight = windowHeight;
	}
	if (windowWidth < 1) {
		if (videoWidth < 1)
			windowWidth = defaultWindowWidth;
		else
			windowWidth = videoWidth;
	}
	if (windowHeight < 1) {
		if (videoHeight < 1)
			windowHeight = defaultWindowHeight;
		else
			windowHeight = videoHeight;
	}
	
	
	
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
		videoWidth, 
		videoHeight
	);
	
	SDL_RenderSetLogicalSize(renderer, videoWidth, videoHeight);
}

