

#define maxArity         8
#define nodePageSize   100
#define stackSize     1000
#define maxFrameHeads  100


typedef int16_t  nodeIndex;
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
typedef struct {
	char      *name;
	nodeIndex  fnDef;// just for argument and variable calls
	int8_t     argRefIndex;//just for argument calls
	uint8_t    arity;
	nodeIndex  arguments[maxArity];//if negative then already evaluated
	evaluator  evaluate;
	UOE        output;
} node;

node  nodes[nodePageSize];


void evalArgs(nodeIndex self) {
	for (int i=0; i < nodes[self].arity; i++)
		nodes[ nodes[self].arguments[i] ].evaluate( nodes[self].arguments[i] );
}




nodeIndex stack[stackSize];//holds the call sources
uint32_t stackPos;






void eval_varDef(nodeIndex self) {
	
	//not sure if this is ever necessary
	
	//nodes[self+1].evaluate(self+1);//self+1 will always be the index of the first argument
	//nodes[self].output = nodes[self+1].output;
}

void eval_fnDef(nodeIndex self) {
	
	//not sure if this is ever necessary
	
	//nodes[self+1].evaluate(self+1);
	//nodes[self].output = nodes[self+1].output;
}



void eval_varCall(nodeIndex self) {
	nodes[self].output = nodes[ nodes[self].fnDef ].output;
}


void eval_fnCall(nodeIndex self) {
	//stackPos++;
	//push the call source (self) to the stack
	//evaluate nodes[self.definition] and get the output
	//pop the stack
	//set argindexes positive
}

void eval_argCall(nodeIndex self) {
	//if argindex from callsource is positive, then evaluate
	
	nodes[self].output = 
		nodes[
			nodes[
				stack[stackPos]
			].arguments[
				nodes[self].argRefIndex
			]
		].output
	;
	
	//set argindex from callsource negative
}


void eval_state(nodeIndex self) {
	//not sure yet if this will be useful at all
}





