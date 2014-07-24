

#define    nodePage        20
#define    nodeNamePage    20
int        nodeSpace     =  0;
nodeIndex  currentNode   = -1;
int        nodeNameSpace =  0;
int        namePos       = -1;

#define    rootNodePage      10
int        rootNodeSpace   =  0;
int        currentRootNode = -1;

#define    frameformPage       10
#define    stateNodePage       10
int        frameformSpace   =   0;
int        currentFrameform =  -1;


void initAllocation(void) {
	nodeSpace = nodePage;
	nodes = malloc( sizeof(node) * nodeSpace );
	
	frameformSpace = frameformPage;
	frameforms = malloc( sizeof(frameform) * frameformSpace );
	
	rootNodeSpace = rootNodePage;
	rootNodes = malloc( sizeof(nodeIndex) * rootNodeSpace );
}

void  inc_currentNode(void) {
	currentNode++;
	if (currentNode == nodeSpace) {
		nodeSpace += nodePage;
		nodes = realloc( nodes, sizeof(node) * nodeSpace );
	}
	
	nodeNameSpace =  0;
	namePos       = -1;
}

void  inc_namePos(void) {
	namePos++;
	if (namePos == nodeNameSpace) {
		nodeNameSpace += nodeNamePage;
		if (!(namePos))
			nodes[currentNode].name = malloc( sizeof(char)*nodeNameSpace );
		else {
			nodes[currentNode].name = realloc(
				nodes[currentNode].name, 
				sizeof(char) * nodeNameSpace
			);
		}
	}
}

void  inc_currentRootNode(void) {
	
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
	
	//free nodes[i].name unless it's from a static stdNode
	for (int i = 0; i <= currentNode; i++) {
		int j = 0;
		for (; j < stdNodeTableLength; j++) {
			if (stdNodeTable[j]->name == nodes[i].name)
				break;
		}
		if (j == stdNodeTableLength)
			free( nodes[i].name );
	}
	
	//free the stateNode arrays and then the frameForm array
	for (int i = 0; i <= currentFrameform; i++)
		free( frameforms[i].stateNodes );
	free( frameforms );
	
	free(nodes);
	free(rootNodes);
}

