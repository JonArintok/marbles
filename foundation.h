

#define maxArity 8

typedef struct {
	double    *data;
	uint32_t   dimensionality;
	uint32_t  *dimensions;
} array;
struct node;
typedef struct {
	uint32_t  *data;
	uint32_t   dimensionality;
	uint32_t  *dimensions;
} nodeArray;
typedef union {
	double     n;//number
	uint32_t   f;//function
	array      N;//number array
	nodeArray  F;//function array
} UOE;         //union of everything
typedef void (*evaluator)(uint32_t);
typedef struct node {
	char      *name;
	char      *typeString;
	bool       evaluated;
	uint8_t    arity;
	uint16_t   arguments[maxArity];
	evaluator  evaluate;
	UOE        output;
} node;


#define nodePageSize 100
node  nodes[nodePageSize];
