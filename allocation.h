
#define    nodePage 16 //for arrays of nodes
#define    namePage 16 //for arrays of chars

int        nodeSpace     =  0;
nodeIndex  curNode       = -1;
int        nodeNameSpace =  0;
int        namePos       = -1;


#define    nodeIndexPage 16 //for arrays of nodeIndex and refs to nodeIndex

int        gRootNodeSpace =  0;
int        gCurRootNode   = -1;

#define    frameformPage 16
int        frameformSpace =  0;
int        curFrameform   = -1;

uint32_t   curLine = 0;//line numbers start at 1, not 0


void init_Allocation(void) {
	nodeSpace = nodePage;
	nodes     = malloc(sizeof(  node  ) * nodeSpace);
	nodesInfo = malloc(sizeof(nodeInfo) * nodeSpace);
	
	frameformSpace = frameformPage;
	frameforms = malloc(sizeof(frameform) * frameformSpace);
	
	gRootNodeSpace = nodeIndexPage;
	gRootNodes = malloc(sizeof(nodeIndex) * gRootNodeSpace);
}

void inc_curNode(void) {
	curNode++;
	if (curNode == nodeSpace) {
		nodeSpace += nodePage;
		nodes      = realloc(nodes,     sizeof(  node  ) * nodeSpace);
		nodesInfo  = realloc(nodesInfo, sizeof(nodeInfo) * nodeSpace);
	}
	
	//initialize fields
	nodes[curNode].definition  = 0;
	nodes[curNode].argRefIndex = 0;
	nodes[curNode].childCount  = 0;
	nodes[curNode].evaluate    = NULL;
	nodes[curNode].output.n    = 0;
	for (int i = 0; i < maxChildren; i++)
		nodes[curNode].children[i] = 0;
	
	nodesInfo[curNode].name  = unnamed;
	nodesInfo[curNode].line  = 0;
	nodesInfo[curNode].level = 0;
	nodesInfo[curNode].arity = 0;
	
	nodeNameSpace =  0;
	namePos       = -1;
}

void inc_namePos(void) {
	namePos++;
	if (namePos == nodeNameSpace) {
		nodeNameSpace += namePage;
		if (!namePos)
			nodesInfo[curNode].name = malloc(sizeof(char) * nodeNameSpace);
		else {
			nodesInfo[curNode].name = realloc(
				nodesInfo[curNode].name, sizeof(char) * nodeNameSpace
			);
		}
	}
}

void inc_gCurRootNode(void) {
	gCurRootNode++;
	if (gCurRootNode == gRootNodeSpace) {
		gRootNodeSpace += nodeIndexPage;
		gRootNodes = realloc(gRootNodes, sizeof(nodeIndex) * gRootNodeSpace);
	}
}

void inc_curFrameform(void) {
	curFrameform++;
	if (curFrameform == frameformSpace) {
		frameformSpace += frameformPage;
		frameforms = realloc(
			frameforms, 
			sizeof(frameform) * frameformSpace
		);
	}
	
	frameforms[curFrameform].curStateNode = -1;
	frameforms[curFrameform].stateNodeSpace = nodeIndexPage;
	frameforms[curFrameform].stateNodes = malloc(
		sizeof(nodeIndex) * frameforms[curFrameform].stateNodeSpace
	);
	frameforms[curFrameform].curRootNode = -1;
	frameforms[curFrameform].rootNodeSpace = nodeIndexPage;
	frameforms[curFrameform].rootNodes = malloc(
		sizeof(nodeIndex) * frameforms[curFrameform].rootNodeSpace
	);
}

void inc_curStateNode(void) {
	frameforms[curFrameform].curStateNode++;
	if (
		frameforms[curFrameform].curStateNode == 
		frameforms[curFrameform].stateNodeSpace
	) {
		frameforms[curFrameform].stateNodeSpace += nodeIndexPage;
		frameforms[curFrameform].stateNodes = realloc(
			frameforms[curFrameform].stateNodes,
			sizeof(nodeIndex) * frameforms[curFrameform].stateNodeSpace
		);
	}
}
void inc_curRootNode(void) {
	frameforms[curFrameform].curRootNode++;
	if (
		frameforms[curFrameform].curRootNode == 
		frameforms[curFrameform].rootNodeSpace
	) {
		frameforms[curFrameform].rootNodeSpace += nodeIndexPage;
		frameforms[curFrameform].rootNodes = realloc(
			frameforms[curFrameform].rootNodes,
			sizeof(nodeIndex) * frameforms[curFrameform].rootNodeSpace
		);
	}
}


void cleanUp(void) {
	
	for (int ffPos = 0; ffPos <= curFrameform; ffPos++) {
		for (int snPos = 0; snPos <= frameforms[ffPos].curStateNode; snPos++) {
			free( nodesInfo[ frameforms[ffPos].stateNodes[snPos] ].name );
			free( nodesInfo[ frameforms[ffPos].rootNodes[snPos] ].name );
		}
		free(frameforms[ffPos].stateNodes);
		free(frameforms[ffPos].rootNodes);
	}
	free(frameforms);
	free(gRootNodes);
	
	for (int rnPos = 0; rnPos <= gCurRootNode; rnPos++)
		free( nodesInfo[ gRootNodes[rnPos] ].name );
	free(nodesInfo);
	free(nodes);
}

