

#define maxArity         8
#define nodePageSize   100
#define stackSize     1000
#define maxFrameHeads  100


typedef uint32_t nodeIndex;
typedef double   number;
//byte
//byte array
typedef struct {
	number    *data;
	uint32_t   dimensionality;
	uint32_t  *dimensions;
	//transformation stack
} numArray;
struct node;
typedef struct {
	nodeIndex *data;
	uint32_t   dimensionality;
	uint32_t  *dimensions;
	//transformation stack
} nodeArray;
typedef union {
	number     n;//number
	nodeIndex  f;//function
	numArray   N;//number array
	nodeArray  F;//function array
} UOE;         //union of everything
typedef void (*evaluator)(nodeIndex toBeEvaluated);
typedef struct node {
	char      *name;
	char      *inTypeString;
	char      *argString;
	char      *outTypeString;
	uint8_t    arity;
	uint16_t   arguments[maxArity];
	evaluator  evaluate;
	UOE        output;
} node;

node  nodes[nodePageSize];


void evaluateBranch(nodeIndex toBeEvaluated) {
	for (int i=0; i < nodes[toBeEvaluated].arity; i++)
		evaluateBranch( nodes[toBeEvaluated].arguments[i] );
	nodes[toBeEvaluated].evaluate(toBeEvaluated);
}





typedef struct stackthing {
	nodeIndex  fnDef;
	nodeIndex  callSource;
};

stackthing stack[stackSize]
uint32_t stackPos;



void eval_varCall(nodeIndex self) {}


void eval_fnCall(nodeIndex self) {
	
	//push the stack
	//if there are arguments, evaluate them
	//evaluate the function
	//pop the stack
}

void eval_argCall(nodeIndex self) {
	//refer to the stack
}

void eval_State(nodeIndex self) {}


typedef struct task {
	int ID;
};//not completely sure exactly what this is yet
typedef struct frameHead {
	char       name;
	nodeIndex  beginning;
	int        length;
	task      *tasks;
	UOE       *outputs;
};

frameHead frameHeads[maxFrameHeads];

