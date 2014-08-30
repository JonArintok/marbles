

#define maxChildren      8
#define stackSize    10000


typedef int16_t nodeIndex;
#define  maxNodeIndex  32767

typedef double  number;
typedef uint8_t byte;

typedef void (*transform)(void *data);
#define _arrayType_(name, elemType)\
typedef struct {\
	elemType  *data;\
	uint64_t   dataSpace;\
	uint64_t   dimensionX;\
	uint64_t   dimensionY;\
	uint64_t   dimensionZ;\
	transform *transforms;\
	uint32_t   transformCount;\
} name;
_arrayType_(numArray,  number)
_arrayType_(byteArray, byte)
_arrayType_(nodeArray, nodeIndex)
typedef union {
	number     n;
	number     n2[2];
	number     n3[3];
	number     n4[4];
	numArray  *N;
	byte       b;
	byte       b2[2];
	byte       b3[3];
	byte       b4[4];
	byteArray *B;
	nodeIndex  f;
	nodeArray *F;
} outType;
typedef outType (*evaluator)(nodeIndex toBeEvaluated);
typedef struct {
	nodeIndex  definition;//for argument and variable calls
	int8_t     argRefIndex;//for argument calls
	int8_t     childCount;//the number of "subnodes", defNodes have 1
	nodeIndex  children[maxChildren];//if negative then already evaluated
	evaluator  evaluate;
	outType    output;
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
	int16_t    curRootNode;
	int16_t    rootNodeSpace;
	nodeIndex *rootNodes;
	//per-frameform outputs
	nodeIndex nextFrameform;
	nodeIndex videoOut;
	nodeIndex audioOut;
} frameform;
char *nextFrameformName = "next num";
char *videoOutName      = "videoOut quobyte..";//type name pending
char *audioOutName      = "audioOut num.";


node      *nodes;
nodeIndex *gRootNodes;
frameform *frameforms;




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



outType stack[stackSize][maxChildren];
int32_t stackPos = -1;



#define _output_(toBeEvaluated)\
	nodes[toBeEvaluated].evaluate(toBeEvaluated);

outType eval_varDef(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
	return nodes[self].output;
}
outType eval_varCall(nodeIndex self) {
	return nodes[ nodes[self].definition ].output;
}

outType eval_fnDef(nodeIndex self) {
	return _output_(self+1)
}
outType eval_fnDefN(nodeIndex self) {
	return _output_(self+1)
}

outType eval_fnCall(nodeIndex self) {
	
	stackPos++;
	
	for (int i = 0; i < nodes[self].childCount; i++) {
		nodeIndex arg = nodes[self].children[i];
		stack[stackPos][i] = _output_(arg)
	}
	
	nodeIndex fnBody = nodes[self].definition + 1;
	return _output_(fnBody)
	
	stackPos--;
}
outType eval_fnCallN(nodeIndex self) {
	nodeIndex fnBody = nodes[self].definition + 1;
	return _output_(fnBody)
}

outType eval_argCall(nodeIndex self) {
	
	//stackTarget should eventually be cached somewhere,
	//but until then...
	int stackTarget = stackPos;
	int peekNode = self-1;
	while (nodes[peekNode].evaluate != eval_fnDef)
		peekNode--;
	nodeIndex callDef = peekNode;
	peekNode++;
	for (; peekNode < self; peekNode++) {
		if (
			nodes[peekNode].definition == callDef &&
			nodesInfo[peekNode].level < nodesInfo[self].level
		) {
			stackTarget--;
		}
	}
	
	return stack[stackTarget][ nodes[self].argRefIndex ];
}


outType eval_stateDef(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
	return nodes[self].output;
}
outType eval_shareDef(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
	return nodes[self].output;
}
outType eval_stateCall(nodeIndex self) {
	return nodes[ nodes[self].definition ].output;
}
outType eval_shareCall(nodeIndex self) {
	return nodes[ nodes[self].definition ].output;
}


char *name_frameformRef = "frameformRef num";
outType eval_frameformRef(nodeIndex self) {
	return nodes[self].output;
}

char *name_numLit = "numLit num";
outType eval_numLit(nodeIndex self) {
	return nodes[self].output;
}

