

#define maxChildren 8

typedef uint16_t nodeIndex;
#define  maxNodeIndex  0xFFFF

typedef float   number;
typedef uint8_t byte;

typedef struct {
	number  *data;
	size_t   dataSize; //in bytes
	uint64_t dimenX;   //in elements
	uint32_t dimenY;
	uint32_t dimenZ;
} numArray;
typedef struct {
	byte    *data;
	size_t   dataSize;
	uint64_t dimenX;
	uint32_t dimenY;
	uint32_t dimenZ;
} byteArray;

typedef union {
	number      n;
	number      nt[8];
	numArray    N;
	byte        b;
	byte        bt[8];
	byteArray   B;
	nodeIndex   f;
} outType;
#define _evalargs_ nodeIndex self, outType fnCallArgs[maxChildren]
typedef outType (*evaluator) (_evalargs_);
typedef struct {
	nodeIndex  definition;//for variable/state/fn calls
	int8_t     argRefIndex;//for argument calls
	int8_t     childCount;//the number of "subnodes", defNodes have 1
	nodeIndex  children[maxChildren];
	evaluator  evaluate;
	outType    cache;
} node;

//for non-performance critical info not needed beyond parsing
typedef struct {
	char    *name; //includes type information and parameter names
	uint32_t line; //this node was found on this line in the source file
	uint8_t  level;//elevation+fold
	int16_t  frameform;//-1 if outside not in frameform
	uint8_t  arity;//for fnDef only
} nodeInfo;
nodeInfo *nodesInfo;


#define maxTokenLength 64
#define maxLineLength 256

typedef struct {
	char       name[maxLineLength];
	uint32_t   line;
	int16_t    curStateNode;
	int16_t    stateNodeSpace;
	nodeIndex *stateNodes;
	outType   *hotState;
	int16_t    curRootNode;
	int16_t    rootNodeSpace;
	nodeIndex *rootNodes;
	//per-frameform outputs
	nodeIndex nextFrameform;
	nodeIndex videoOut;
	nodeIndex audioOut;
} frameform;
char *nextFrameformName = "next N1";
char *videoOutName      = "videoOut B4D2";
char *audioOutName      = "audioOut N1'";


node      *nodes;
nodeIndex *gRootNodes;
frameform *frameforms;
int        curNode      = -1;
int        gCurRootNode = -1;
int        curFrameform = -1;


typedef struct {
	char     *name;
	uint8_t   arity;
	evaluator evaluate;
} stdNode;

const stdNode *stdNodeTable[];



//global outputs
double frameRate    = -1;
int    videoWidth   = 0;
int    videoHeight  = 0;
int    windowWidth  = 0;
int    windowHeight = 0;
nodeIndex frameRateRoot    = maxNodeIndex;
nodeIndex videoWidthRoot   = maxNodeIndex;
nodeIndex videoHeightRoot  = maxNodeIndex;
nodeIndex windowWidthRoot  = maxNodeIndex;
nodeIndex windowHeightRoot = maxNodeIndex;
char *frameRateName    = "frameRate N1";
char *videoWidthName   = "videoWidth N1";
char *videoHeightName  = "videoHeight N1";
char *windowWidthName  = "windowWidth N1";
char *windowHeightName = "windowHeight N1";



#define _output_(toBeEvaluated, fnCallArgs)\
	nodes[toBeEvaluated].evaluate(toBeEvaluated, fnCallArgs);

outType eval_varDef(_evalargs_) {
	return nodes[self].cache;
}
outType eval_varCall(_evalargs_) {
	return nodes[ nodes[self].definition ].cache;
}

outType eval_fnDef(_evalargs_) {
	return _output_(self+1, fnCallArgs)
}
outType eval_fnDefN(_evalargs_) {
	return _output_(self+1, fnCallArgs)
}

outType eval_fnCall(_evalargs_) {
	outType newFnCallArgs[maxChildren];
	for (int i = 0; i < nodes[self].childCount; i++) {
		nodeIndex arg = nodes[self].children[i];
		newFnCallArgs[i] = nodes[arg].evaluate(arg, fnCallArgs);
	}
	nodeIndex fnBody = nodes[self].definition + 1;
	return _output_(fnBody, newFnCallArgs)
}
outType eval_fnCallN(_evalargs_) {
	nodeIndex fnBody = nodes[self].definition + 1;
	return _output_(fnBody, fnCallArgs)
}

outType eval_argCall(_evalargs_) {
	return fnCallArgs[ nodes[self].argRefIndex ];
}
outType eval_fnArgCall(_evalargs_) {
	nodeIndex nodePassed = fnCallArgs[ nodes[self].argRefIndex ].f;
	
	//std fn
	if (nodePassed > curNode)
		return stdNodeTable[nodePassed-curNode-1]->evaluate(self, fnCallArgs);
	
	//user-defined fn
	outType newFnCallArgs[maxChildren];
	for (int i = 0; i < nodes[self].childCount; i++) {
		nodeIndex arg = nodes[self].children[i];
		newFnCallArgs[i] = nodes[arg].evaluate(arg, fnCallArgs);
	}
	return _output_(nodePassed+1, newFnCallArgs)
}
outType eval_fnPass(_evalargs_) {
	return nodes[self].cache;
}


outType eval_stateDef(_evalargs_) {
	return nodes[self].cache;
}
outType eval_shareDef(_evalargs_) {
	return nodes[self].cache;
}
outType eval_stateCall(_evalargs_) {
	return nodes[ nodes[self].definition ].cache;
}
outType eval_shareCall(_evalargs_) {
	return nodes[ nodes[self].definition ].cache;
}


outType eval_gOutDef(_evalargs_) {
	return nodes[self].cache;
}

outType eval_outDef(_evalargs_) {
	return nodes[self].cache;
}


char *name_frameformRef = "frameformRef N1";
outType eval_frameformRef(_evalargs_) {
	return nodes[self].cache;
}

char *name_numLit = "numLit N1";
outType eval_numLit(_evalargs_) {
	return nodes[self].cache;
}



bool isReadOnly(nodeIndex n) {
	return 
		nodes[n].evaluate == eval_varCall ||
		nodes[n].evaluate == eval_stateCall;
}
