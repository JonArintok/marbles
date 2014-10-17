

#define maxChildren 8

typedef uint16_t nodeIndex;
#define  maxNodeIndex  65535

typedef double  number;
typedef uint8_t byte;

#define _arrayType_(name, elemType)\
typedef struct {\
	elemType  *data;\
	uint64_t   dataSpace;\
	uint64_t   dimensionX;\
	uint32_t   dimensionY;\
	uint32_t   dimensionZ;\
} name;
_arrayType_(numArray,  number)
_arrayType_(byteArray, byte)
_arrayType_(nodeArray, nodeIndex)
typedef union {
	number      n;
	number      n2[2];
	number      n3[3];
	number      n4[4];
	numArray    N;
	byte        b;
	byte        b2[2];
	byte        b4[4];
	byte        b8[8];
	byte        b16[16];
	byteArray   B;
	nodeIndex   f;
	nodeArray   F;
} outType;
typedef outType (*evaluator) (
	nodeIndex toBeEvaluated,
	outType fnCallArgs[maxChildren]
);
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
char *nextFrameformName = "next num";
char *videoOutName      = "videoOut byte4**";
char *audioOutName      = "audioOut num.";


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

#define stdNodeTableLength  15
const stdNode *stdNodeTable[stdNodeTableLength];



//global outputs
double frameRate    = defaultFrameRate;
double windowWidth  = defaultWindowWidth;
double windowHeight = defaultWindowHeight;
nodeIndex frameRateRoot    = -1;
nodeIndex windowWidthRoot  = -1;
nodeIndex windowHeightRoot = -1;
char *frameRateName    = "frameRate num";
char *windowWidthName  = "windowWidth num";
char *windowHeightName = "windowHeight num";



#define _output_(toBeEvaluated, fnCallArgs)\
	nodes[toBeEvaluated].evaluate(toBeEvaluated, fnCallArgs);

outType eval_varDef(nodeIndex self, outType fnCallArgs[maxChildren]) {
	nodes[self].cache = nodes[self+1].cache;
	return nodes[self].cache;
}
outType eval_varCall(nodeIndex self, outType fnCallArgs[maxChildren]) {
	return nodes[ nodes[self].definition ].cache;
}

outType eval_fnDef(nodeIndex self, outType fnCallArgs[maxChildren]) {
	return _output_(self+1, fnCallArgs)
}
outType eval_fnDefN(nodeIndex self, outType fnCallArgs[maxChildren]) {
	return _output_(self+1, fnCallArgs)
}

outType eval_fnCall(nodeIndex self, outType fnCallArgs[maxChildren]) {
	outType newFnCallArgs[maxChildren];
	for (int i = 0; i < nodes[self].childCount; i++) {
		nodeIndex arg = nodes[self].children[i];
		newFnCallArgs[i] = nodes[arg].evaluate(arg, fnCallArgs);
	}
	nodeIndex fnBody = nodes[self].definition + 1;
	return _output_(fnBody, newFnCallArgs)
}
outType eval_fnCallN(nodeIndex self, outType fnCallArgs[maxChildren]) {
	nodeIndex fnBody = nodes[self].definition + 1;
	return _output_(fnBody, fnCallArgs)
}

outType eval_argCall(nodeIndex self, outType fnCallArgs[maxChildren]) {
	return fnCallArgs[ nodes[self].argRefIndex ];
}
outType eval_fnArgCall(nodeIndex self, outType fnCallArgs[maxChildren]) {
	//std fn
	nodeIndex nodePassed = fnCallArgs[ nodes[self].argRefIndex ].f;
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
outType eval_fnPass(nodeIndex self, outType fnCallArgs[maxChildren]) {
	return nodes[self].cache;
}


outType eval_stateDef(nodeIndex self, outType fnCallArgs[maxChildren]) {
	return nodes[self].cache;
}
outType eval_shareDef(nodeIndex self, outType fnCallArgs[maxChildren]) {
	return nodes[self].cache;
}
outType eval_stateCall(nodeIndex self, outType fnCallArgs[maxChildren]) {
	return nodes[ nodes[self].definition ].cache;
}
outType eval_shareCall(nodeIndex self, outType fnCallArgs[maxChildren]) {
	return nodes[ nodes[self].definition ].cache;
}


char *name_frameformRef = "frameformRef num";
outType eval_frameformRef(nodeIndex self, outType fnCallArgs[maxChildren]) {
	return nodes[self].cache;
}

char *name_numLit = "numLit num";
outType eval_numLit(nodeIndex self, outType fnCallArgs[maxChildren]) {
	return nodes[self].cache;
}

