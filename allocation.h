

#define    nodePage        20
#define    nodeNamePage    16
int        nodeSpace     =  0;
nodeIndex  currentNode   = -1;
int        nodeNameSpace =  0;
int        namePos       = -1;

#define    rootNodePage      16
int        rootNodeSpace   =  0;
int        currentRootNode = -1;

#define    frameformPage       10
#define    stateNodePage       16
int        frameformSpace   =   0;
int        currentFrameform =  -1;


void init_Allocation(void) {
	nodeSpace = nodePage;
	nodes = malloc(sizeof(node) * nodeSpace);
	
	frameformSpace = frameformPage;
	frameforms = malloc(sizeof(frameform) * frameformSpace);
	
	rootNodeSpace = rootNodePage;
	rootNodes = malloc(sizeof(nodeIndex) * rootNodeSpace);
}

void inc_currentNode(void) {
	currentNode++;
	if (currentNode == nodeSpace) {
		nodeSpace += nodePage;
		nodes = realloc(nodes, sizeof(node) * nodeSpace);
	}
	
	//initialize fields
	nodes[currentNode].name        = initialName;
	nodes[currentNode].definition  = 0;
	nodes[currentNode].argRefIndex = 0;
	nodes[currentNode].arity       = 0;
	nodes[currentNode].evaluate    = NULL;
	nodes[currentNode].output.n    = 0;
	for (int i = 0; i < maxArity; i++)
		nodes[currentNode].arguments[i] = 0;
	
	
	nodeNameSpace =  0;
	namePos       = -1;
}

void inc_namePos(void) {
	namePos++;
	if (namePos == nodeNameSpace) {
		nodeNameSpace += nodeNamePage;
		if (!(namePos))
			nodes[currentNode].name = malloc(sizeof(char) * nodeNameSpace);
		else {
			nodes[currentNode].name = realloc(
				nodes[currentNode].name, 
				sizeof(char) * nodeNameSpace
			);
		}
	}
}

void inc_currentRootNode(void) {
	currentRootNode++;
	if (currentRootNode == rootNodeSpace) {
		rootNodeSpace += rootNodePage;
		rootNodes = realloc(rootNodes, sizeof(nodeIndex) * rootNodeSpace);
	}
}

void  inc_currentFrameform(void) {
	currentFrameform++;
	if (currentFrameform == frameformSpace) {
		frameformSpace += frameformPage;
		frameforms = realloc(
			frameforms, 
			sizeof(frameform) * frameformSpace
		);
	}
	
	frameforms[currentFrameform].currentStateNode = -1;
	frameforms[currentFrameform].stateNodeSpace = stateNodePage;
	frameforms[currentFrameform].stateNodes = malloc(
		sizeof(nodeIndex) * frameforms[currentFrameform].stateNodeSpace
	);
}

void  inc_currentStateNode(void) {
	frameforms[currentFrameform].currentStateNode++;
	if (
		frameforms[currentFrameform].currentStateNode == 
		frameforms[currentFrameform].stateNodeSpace
	) {
		frameforms[currentFrameform].stateNodeSpace += stateNodePage;
		frameforms[currentFrameform].stateNodes = realloc(
			frameforms[currentFrameform].stateNodes,
			sizeof(nodeIndex) * frameforms[
				currentFrameform
			].stateNodeSpace
		);
	}
}


void cleanUp(void) {
	//free the names of the rootNodes, then the rootNodes themselves
	for (int i = 0; i <= currentRootNode; i++)
		free( nodes[ rootNodes[i] ].name );
	free(rootNodes);
	
	//free the names of the stateNodes of the frameforms, 
	//then the stateNodes themselves, then the frameforms themselves
	for (int i = 0; i <= currentFrameform; i++) {
		for (int j = 0; j <= frameforms[i].currentStateNode; j++)
			free ( nodes[ frameforms[i].stateNodes[j] ].name );
		free( frameforms[i].stateNodes );
	}
	free( frameforms );
	
	//free the nodes themselves
	free(nodes);
}

