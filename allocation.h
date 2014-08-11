

#define    nodePage        16
#define    nodeNamePage    16
int        nodeSpace     =  0;
nodeIndex  currentNode   = -1;
int        nodeNameSpace =  0;
int        namePos       = -1;

#define    rootNodePage      16
int        rootNodeSpace   =  0;
int        currentRootNode = -1;

#define    frameformPage       16
#define    stateNodePage       16
int        frameformSpace   =   0;
int        currentFrameform =  -1;

uint32_t   currentLine = 0;//line numbers start at 1, not 0

typedef struct {
	char    *name; //includes type information and parameter names
	uint32_t line; //this node was found on this line in the source file
	uint32_t level;//elevation+fold
} nodeInfo;
nodeInfo *nodesInfo;


void init_Allocation(void) {
	nodeSpace = nodePage;
	nodes     = malloc(sizeof(  node  ) * nodeSpace);
	nodesInfo = malloc(sizeof(nodeInfo) * nodeSpace);
	
	frameformSpace = frameformPage;
	frameforms = malloc(sizeof(frameform) * frameformSpace);
	
	rootNodeSpace = rootNodePage;
	rootNodes = malloc(sizeof(nodeIndex) * rootNodeSpace);
}

void inc_currentNode(void) {
	currentNode++;
	if (currentNode == nodeSpace) {
		nodeSpace += nodePage;
		nodes      = realloc(nodes,     sizeof(  node  ) * nodeSpace);
		nodesInfo  = realloc(nodesInfo, sizeof(nodeInfo) * nodeSpace);
	}
	
	//initialize fields
	nodes[currentNode].definition  = 0;
	nodes[currentNode].argRefIndex = 0;
	nodes[currentNode].arity       = 0;
	nodes[currentNode].evaluate    = NULL;
	nodes[currentNode].output.n    = 0;
	for (int i = 0; i < maxArity; i++)
		nodes[currentNode].arguments[i] = 0;
	
	nodesInfo[currentNode].name  = initialName;
	nodesInfo[currentNode].line  = 0;
	nodesInfo[currentNode].level = 0;
	
	nodeNameSpace =  0;
	namePos       = -1;
}

void inc_namePos(void) {
	namePos++;
	if (namePos == nodeNameSpace) {
		nodeNameSpace += nodeNamePage;
		if (!(namePos))
			nodesInfo[currentNode].name = malloc(sizeof(char) * nodeNameSpace);
		else {
			nodesInfo[currentNode].name = realloc(
				nodesInfo[currentNode].name, 
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

void inc_currentFrameform(void) {
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

void inc_currentStateNode(void) {
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
	
	for (int i = 0; i <= currentFrameform; i++)
		free(frameforms[i].stateNodes);
	free(frameforms);
	free(rootNodes);
	
	for (int i = 0; i <= currentNode; i++)
		free(nodesInfo[i].name);
	free(nodesInfo);
	free(nodes);
}

