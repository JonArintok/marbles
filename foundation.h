

#define maxArity         8
#define stackSize     1000


typedef int16_t  nodeIndex;
#define  maxNodeIndex  32767

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
	nodeIndex  definition;// just for argument and variable calls
	int8_t     argRefIndex;//just for argument calls
	uint8_t    arity;
	nodeIndex  arguments[maxArity];//if negative then already evaluated
	evaluator  evaluate;
	UOE        output;
} node;

typedef struct frame {
	char       *name;
	int16_t     outputCount;
	nodeIndex  *outputs;
};


nodeIndex *nodes;
nodeIndex *rootNodes;
nodeIndex *frames;



//void evalArgs(nodeIndex self) {
//	for (int i=0; i < nodes[self].arity; i++)
//		nodes[ nodes[self].arguments[i] ].evaluate( nodes[self].arguments[i] );
//}




nodeIndex stack[stackSize];//holds the call sources
uint32_t stackPos;




void eval_varDef(nodeIndex self) {}

void eval_fnDef(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
}

void eval_state(nodeIndex self) {
	nodes[self].output = nodes[self+1].output;
}



void eval_varCall(nodeIndex self) {
	nodes[self].output = nodes[ nodes[self].definition ].output;
}


void eval_fnCall(nodeIndex self) {
	stackPos++;
	stack[stackPos] = self;
	
	nodeIndex fnBody = nodes[self].definition + 1;
	//evaluate nodes[self.definition] and get the output
	nodes[fnBody].evaluate(fnBody);
	nodes[self].output = nodes[fnBody].output;
	
	//reset argument values to positive
	for (int i=0; i<nodes[self].arity; i++)
		nodes[fnBody-1].arguments[i] &= maxNodeIndex;
	
	stackPos--;
}

void eval_argCall(nodeIndex self) {
	//get the nodeIndex of the argument which will have the value we want
	nodeIndex argNodeIndex = 
		nodes[ stack[stackPos] ].arguments[ nodes[self].argRefIndex ];
	
	//if argNodeIndex is positive, then we need to evaluate
	if (argNodeIndex >= 0) {
		nodes[argNodeIndex].evaluate(argNodeIndex);
		nodes[self].output = nodes[argNodeIndex].output;
		
		//set nodeIndex from the callsource's argument list negative
		nodes[ stack[stackPos] ].arguments[ nodes[self].argRefIndex ] *= -1;
	}
	else
		nodes[self].output = nodes[ argNodeIndex * -1 ].output;
}





