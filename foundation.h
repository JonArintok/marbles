

#define maxArity 8

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
	//int        nodeID;
	char      *name;
	char      *inTypeString;
	char      *outTypeString;
	uint8_t    arity;
	uint16_t   arguments[maxArity];
	evaluator  evaluate;
	UOE        output;
} node;

#define nodePageSize 100
node  nodes[nodePageSize];


void evaluateBranch(nodeIndex toBeEvaluated) {
	for (int i=0; i < nodes[toBeEvaluated].arity; i++)
		evaluateBranch( nodes[toBeEvaluated].arguments[i] );
	nodes[toBeEvaluated].evaluate(toBeEvaluated);
}


/*


typedef struct stackthing {
	uint32_t  fnDef;
	uint32_t  callSource;
};

#define stacksize
stackthing stack[stacksize]
uint32_t stackPos;



eval_varCall
	//nothing to do, value only ever changes between frames or on edit


eval_fnCall
	
	push the stack
	if there are arguments, evaluate them
	evaluate the function
	pop the stack
	



eval_argCall
	
	refer to the stack



*/

