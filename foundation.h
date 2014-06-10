

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
	//char      *inTypeString;
	//char      *argString;
	//char      *outTypeString;
	nodeIndex  fnDef;// just for argument and variable calls
	int8_t     argRefIndex;//just for argument calls
	uint8_t    arity;
	nodeIndex  arguments[maxArity];//if negative then already evaluated
	evaluator  evaluate;
	UOE        output;
};

node  nodes[nodePageSize];


void evaluateBranch(nodeIndex toBeEvaluated) {
	for (int i=0; i < nodes[toBeEvaluated].arity; i++)
		evaluateBranch( nodes[toBeEvaluated].arguments[i] );
	nodes[toBeEvaluated].evaluate(toBeEvaluated);
}
// there is a problem with that 'for' statement which is that 
// it might evaluate arguments that need not be evaluated.
// (consider the "if" function)





nodeIndex stack[stackSize]//holds the call sources
uint32_t stackPos;




typedef struct task {
	int ID;
};//not completely sure exactly what this is yet
typedef struct frameHead {
	char       name;
	nodeIndex  beginning;
	int        length;
	task      *tasks;
	UOE       *writeBuffers;
};

frameHead frameHeads[maxFrameHeads];









void eval_varDef(nodeIndex self) {
	//not sure yet if this will be useful at all
}

void eval_varCall(nodeIndex self) {
	//refer to the varDef
}



void eval_fnDef(nodeIndex self) {
	//not sure yet if this will be useful at all
}


void eval_fnCall(nodeIndex self) {
	//push the call source (self) to the stack
	//currentStackIndex++;
	//evaluate nodes[self.definition] and get the output
	//pop the stack
	//set argindexes positive
}

void eval_argCall(nodeIndex self) {
	//if argindex from callsource is positive, then evaluate
	//"return":
	nodes[
		nodes[
			stack[currentStackIndex].callSource
		].arguments[
			nodes[self].argRefIndex
		]
	].output;
	//set argindex from callsource negative
}


void eval_state(nodeIndex self) {
	//not sure yet if this will be useful at all
}





