

#define maxArity         8
#define stackSize     1000


typedef int16_t  nodeIndex;
#define  maxNodeIndex  32767

typedef double   number;
//byte
//byte array
typedef struct {
	uint32_t   dimensions[2];
	number    *data;
	//transformation stack
} numArray;
struct node;
typedef struct {
	uint32_t   dimensions[2];
	nodeIndex *data;
	//transformation stack
} nodeArray;
typedef union {
	number     n;//number
	nodeIndex  f;//function
	numArray   N;//number array
	nodeArray  F;//function array
} outType;
typedef void (*evaluator)(nodeIndex toBeEvaluated);
typedef struct {
	char      *name;       //includes type information and parameter names
	nodeIndex  definition; //just for argument and variable calls
	int8_t     argRefIndex;//just for argument calls
	int8_t     arity;      //the number of "subnodes", defNodes have 1
	nodeIndex  arguments[maxArity];//if negative then already evaluated
	evaluator  evaluate;
	outType    output;
} node;



#define   maxTokenLength      80
#define   maxLineLength      200

typedef struct {
	char        name[maxTokenLength];
	int16_t     currentStateNode;
	int16_t     stateNodeSpace;
	nodeIndex  *stateNodes;
} frameform;


node      *nodes;
nodeIndex *rootNodes;
frameform *frameforms;



void evaluateNode(nodeIndex toBeEvaluated) {
	nodes[toBeEvaluated].evaluate(toBeEvaluated);
}
void evalArgs(nodeIndex self) {
	for (int i=0; i < nodes[self].arity; i++)
		evaluateNode( nodes[self].arguments[i] );
}



nodeIndex stack[stackSize];//holds the call sources
uint32_t stackPos;



void eval_varDef(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
}

void eval_varCall(nodeIndex self) {
	nodes[self].output = nodes[ nodes[self].definition ].output;
}


void eval_fnDef(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
}

void eval_fnCall(nodeIndex self) {
	stackPos++;
	stack[stackPos] = self;
	
	nodeIndex fnBody = nodes[self].definition + 1;
	//evaluate nodes[self.definition] and get the output
	evaluateNode(fnBody);
	nodes[self].output = nodes[fnBody].output;
	
	//reset argument values to positive
	for (int i = 0; i < nodes[self].arity; i++)
		nodes[fnBody-1].arguments[i] &= maxNodeIndex;
	
	stackPos--;
}

void eval_argCall(nodeIndex self) {
	//get the nodeIndex of the argument which will have the value we want
	nodeIndex argNodeIndex = 
		nodes[ stack[stackPos] ].arguments[ nodes[self].argRefIndex ];
	
	//if argNodeIndex is positive, then we need to evaluate
	if (argNodeIndex >= 0) {
		evaluateNode(argNodeIndex);
		nodes[self].output = nodes[argNodeIndex].output;
		
		//set nodeIndex from the callsource's argument list negative
		nodes[ stack[stackPos] ].arguments[ nodes[self].argRefIndex ] *= -1;
	}
	else
		nodes[self].output = nodes[ argNodeIndex * -1 ].output;
}


void eval_stateDef(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
}

void eval_stateCall(nodeIndex self) {
	nodes[self].output = nodes[ nodes[self].definition ].output;
}

void eval_numLit(nodeIndex self) {}



int currentFrame = -1;

char *initialName = "! ! unnamed ! !";


//this value will eventually be taken from the .marbles source to be run
number run_fps = 10;


