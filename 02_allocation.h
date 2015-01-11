


int        nodeSpace     =  0;
int        nodeNameSpace =  0;
int        namePos       = -1;

int        gRootNodeSpace =  0;

int        frameformSpace =  0;

int        loadedNodeSpace = 0;
int        curLoadedNode = -1;
nodeIndex *loadedNodes;


char *unnamed = "!!  unnamed  !!";

uint32_t   curLine = 0;//line numbers start at 1, not 0


void init_Allocation(void) {
	nodeSpace = 32;
	nodes     = malloc(sizeof(node    ) * nodeSpace);
	nodesInfo = malloc(sizeof(nodeInfo) * nodeSpace);
	
	frameformSpace = 32;
	frameforms = malloc(sizeof(frameform) * frameformSpace);
	
	gRootNodeSpace = 32;
	gRootNodes = malloc(sizeof(nodeIndex) * gRootNodeSpace);
	
	loadedNodeSpace = 32;
	loadedNodes = malloc(sizeof(nodeIndex) * loadedNodeSpace);
}

void inc_curNode(void) {
	curNode++;
	if (curNode == nodeSpace) {
		nodeSpace *= 2;
		nodes      = realloc(nodes,     sizeof(node    ) * nodeSpace);
		nodesInfo  = realloc(nodesInfo, sizeof(nodeInfo) * nodeSpace);
	}
	
	//initialize fields
	nodes[curNode].def         = maxNodeIndex;
	nodes[curNode].argRefIndex = 0;
	nodes[curNode].childCount  = 0;
	nodes[curNode].evaluate    = NULL;
	for (int i = 0; i < maxChildren; i++)
		nodes[curNode].children[i] = maxNodeIndex;
	memset(&nodes[curNode].cache, 0, sizeof(outType));
	
	nodesInfo[curNode].name  = unnamed;
	nodesInfo[curNode].line  = 0;
	nodesInfo[curNode].level = 0;
	nodesInfo[curNode].arity = 0;
	nodesInfo[curNode].frameform = -1;
	
	nodeNameSpace = 16;
	nodesInfo[curNode].name = malloc(sizeof(char) * nodeNameSpace);
	namePos = -1;
}

void inc_namePos(void) {
	namePos++;
	if (namePos == nodeNameSpace) {
		nodeNameSpace *= 2;
		nodesInfo[curNode].name = 
			realloc(nodesInfo[curNode].name, sizeof(char) * nodeNameSpace);
	}
}

void inc_gCurRootNode(void) {
	gCurRootNode++;
	if (gCurRootNode == gRootNodeSpace) {
		gRootNodeSpace *= 2;
		gRootNodes = realloc(gRootNodes, sizeof(nodeIndex) * gRootNodeSpace);
	}
}

void inc_curFrameform(void) {
	curFrameform++;
	if (curFrameform == frameformSpace) {
		frameformSpace *= 2;
		frameforms = realloc(frameforms, sizeof(frameform) * frameformSpace);
	}
	
	frameforms[curFrameform].curStateNode   = maxNodeIndex;
	frameforms[curFrameform].stateNodeSpace = 16;
	frameforms[curFrameform].stateNodes = 
		malloc(sizeof(nodeIndex) * frameforms[curFrameform].stateNodeSpace);
	frameforms[curFrameform].hotState = 
		malloc(sizeof(outType) * frameforms[curFrameform].stateNodeSpace);
	
	frameforms[curFrameform].curRootNode   = maxNodeIndex;
	frameforms[curFrameform].rootNodeSpace = 16;
	frameforms[curFrameform].rootNodes = 
		malloc(sizeof(nodeIndex) * frameforms[curFrameform].rootNodeSpace);
	
	frameforms[curFrameform].nextFrameform = maxNodeIndex;
	frameforms[curFrameform].videoOut      = maxNodeIndex;
	frameforms[curFrameform].audioOut      = maxNodeIndex;
}

void inc_curStateNode(void) {
	frameforms[curFrameform].curStateNode++;
	if (
		frameforms[curFrameform].curStateNode == 
		frameforms[curFrameform].stateNodeSpace
	) {
		frameforms[curFrameform].stateNodeSpace *= 2;
		frameforms[curFrameform].stateNodes = realloc(
			frameforms[curFrameform].stateNodes,
			sizeof(nodeIndex) * frameforms[curFrameform].stateNodeSpace
		);
		frameforms[curFrameform].hotState = realloc(
			frameforms[curFrameform].hotState,
			sizeof(outType) * frameforms[curFrameform].stateNodeSpace
		);
	}
}
void inc_curRootNode(void) {
	frameforms[curFrameform].curRootNode++;
	if (
		frameforms[curFrameform].curRootNode == 
		frameforms[curFrameform].rootNodeSpace
	) {
		frameforms[curFrameform].rootNodeSpace *= 2;
		frameforms[curFrameform].rootNodes = realloc(
			frameforms[curFrameform].rootNodes,
			sizeof(nodeIndex) * frameforms[curFrameform].rootNodeSpace
		);
	}
}

void addLoadedNode(nodeIndex n) {
	curLoadedNode++;
	if (curLoadedNode == loadedNodeSpace) {
		loadedNodeSpace *= 2;
		loadedNodes = realloc(loadedNodes, sizeof(nodeIndex) * loadedNodeSpace);
	}
	loadedNodes[curLoadedNode] = n;
}

void setLoadedNode(nodeIndex self, size_t newDataSize) {
	if (!nodes[self].cache.B.data) {
		nodes[self].cache.B.data = malloc(newDataSize);
		nodes[self].cache.B.dataSize = newDataSize;
		memset(nodes[self].cache.B.data, 0, newDataSize);
		addLoadedNode(self);
	}
	else if (nodes[self].cache.B.dataSize < newDataSize) {
		nodes[self].cache.B.data = 
			realloc(nodes[self].cache.B.data, newDataSize);
		nodes[self].cache.B.dataSize = newDataSize;
	}
}


void calledAtExit(void) {
	
	for (int lnPos = 0; lnPos <= curLoadedNode; lnPos++)
		free( (void*) nodes[ loadedNodes[lnPos] ].cache.B.data );
	
	for (int ffPos = 0; ffPos <= curFrameform; ffPos++) {
		free(frameforms[ffPos].hotState);
		for (int snPos = 0; snPos <= frameforms[ffPos].curStateNode; snPos++)
			free( nodesInfo[ frameforms[ffPos].stateNodes[snPos] ].name );
		free(frameforms[ffPos].stateNodes);
		for (int rnPos = 0; rnPos <= frameforms[ffPos].curRootNode; rnPos++)
			free( nodesInfo[ frameforms[ffPos].rootNodes[rnPos] ].name );
		free(frameforms[ffPos].rootNodes);
	}
	free(frameforms);
	
	for (int rnPos = 0; rnPos <= gCurRootNode; rnPos++)
		free( nodesInfo[ gRootNodes[rnPos] ].name );
	free(gRootNodes);
	
	free(nodesInfo);
	free(nodes);
	
	puts("finished.");
}

