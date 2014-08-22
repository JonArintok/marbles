

#define maxChildren      8
#define stackSize     1000


typedef int16_t nodeIndex;
#define  maxNodeIndex  32767

typedef double  number;
typedef uint8_t byte;

#define _arrayType_(name, elemType)\
typedef struct {\
	elemType  *data;\
	uint64_t   dataSpace;\
	uint64_t   length;\
	nodeIndex *transformations;\
} name;
_arrayType_(numArray,  number)
_arrayType_(byteArray, byte)
_arrayType_(nodeArray, nodeIndex)
typedef union {
	number    n;
	number    n2[2];
	number    n3[3];
	number    n4[4];
	numArray  N;
	byte      b;
	byte      b2[2];
	byte      b3[3];
	byte      b4[4];
	byteArray B;
	nodeIndex f;
	nodeArray F;
} outType;
typedef void (*evaluator)(nodeIndex toBeEvaluated);
typedef struct {
	nodeIndex  definition;//for argument and variable calls
	int8_t     argRefIndex;//for argument calls
	int8_t     childCount;//the number of "subnodes", defNodes have 1
	nodeIndex  children[maxChildren];//if negative then already evaluated
	evaluator  evaluate;
	outType    output;
} node;



#define maxTokenLength 64
#define maxLineLength 256

typedef struct {
	char       name[maxLineLength];
	int16_t    curStateNode;
	int16_t    stateNodeSpace;
	nodeIndex *stateNodes;
	int16_t    curRootNode;
	int16_t    rootNodeSpace;
	nodeIndex *rootNodes;
	//output
	nodeIndex nextFrameform;
	nodeIndex frameRate;
	nodeIndex videoOut;
	nodeIndex audioOut;
} frameform;

char *nextFrameformName = "nextFrameform num";
char *frameRateName     = "frameRate num";
char *videoOutName      = "videoOut byte||";
char *audioOutName      = "audioOut num|";


node      *nodes;
nodeIndex *gRootNodes;
frameform *frameforms;



#define _evaluateNode_(toBeEvaluated)\
	nodes[toBeEvaluated].evaluate(toBeEvaluated);


nodeIndex stack[stackSize];//holds the call sources
uint32_t stackPos;


void eval_numLit(nodeIndex self) {}

void eval_varDef(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
}
void eval_varCall(nodeIndex self) {
	nodes[self].output = nodes[ nodes[self].definition ].output;
}

void eval_fnDef(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
}
void eval_fnDefN(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
}

void eval_fnCall(nodeIndex self) {
	stackPos++;
	stack[stackPos] = self;
	
	nodeIndex fnBody = nodes[self].definition + 1;
	//evaluate nodes[self.definition] and get the output
	_evaluateNode_(fnBody)
	nodes[self].output = nodes[fnBody].output;
	
	//reset argument values to positive
	for (int i = 0; i < nodes[self].childCount; i++)
		nodes[fnBody-1].children[i] &= maxNodeIndex;
	
	stackPos--;
}
void eval_fnCallN(nodeIndex self) {
	stackPos++;
	stack[stackPos] = self;
	
	nodeIndex fnBody = nodes[self].definition + 1;
	//evaluate nodes[self.definition] and get the output
	_evaluateNode_(fnBody)
	nodes[self].output = nodes[fnBody].output;
	
	stackPos--;
}

void eval_argCall(nodeIndex self) {
	//get the nodeIndex of the argument which will have the value we want
	nodeIndex argNodeIndex = 
		nodes[ stack[stackPos] ].children[ nodes[self].argRefIndex ];
	
	//if argNodeIndex is positive, then we need to evaluate
	if (argNodeIndex >= 0) {
		_evaluateNode_(argNodeIndex)
		nodes[self].output = nodes[argNodeIndex].output;
		
		//set nodeIndex from the callsource's argument list negative
		nodes[ stack[stackPos] ].children[ nodes[self].argRefIndex ] *= -1;
	}
	else
		nodes[self].output = nodes[ argNodeIndex * -1 ].output;
}


void eval_stateDef(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
}
void eval_shareDef(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
}
void eval_stateCall(nodeIndex self) {
	nodes[self].output = nodes[ nodes[self].definition ].output;
}
void eval_shareCall(nodeIndex self) {
	nodes[self].output = nodes[ nodes[self].definition ].output;
}

void eval_outDef(nodeIndex self) {}


int curFrame = -1;

char *unnamed = "!!  unnamed  !!";

bool audioEnabled = false;
bool videoEnabled = false;


int  activeFrameform = 0;



