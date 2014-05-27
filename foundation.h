

#define maxArity 8

//byte
//byte array
typedef struct {
	double    *data;
	uint32_t   dimensionality;
	uint32_t  *dimensions;
	//transformation stack
} array;
struct node;
typedef struct {
	uint32_t  *data;
	uint32_t   dimensionality;
	uint32_t  *dimensions;
	//transformation stack
} nodeArray;
typedef union {
	double     n;//number
	uint32_t   f;//function
	array      N;//number array
	nodeArray  F;//function array
} UOE;         //union of everything
typedef struct node {
	int        nodeID;
	char      *name;
	char      *typeString;
	uint8_t    arity;
	uint16_t   arguments[maxArity];
	UOE        output;
} node;


#define nodePageSize 100
node  nodes[nodePageSize];

