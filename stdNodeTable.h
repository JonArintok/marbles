

typedef struct {
	char     *name;
	uint8_t   arity;
	evaluator evaluate;
} stdNode;

#define _evalArgs_\
	for (int i=0; i < nodes[self].childCount; i++)\
		_evaluateNode_( nodes[self].children[i] )\

#define _evalArith_(name, op, type) \
void name(nodeIndex self) {\
	_evalArgs_\
	nodes[self].output.type\
		= nodes[ nodes[self].children[0] ].output.type\
		op nodes[ nodes[self].children[1] ].output.type\
	;\
}

#define  _arithType_  " num\na num\nb num"

_evalArith_(eval_add, +, n)
const stdNode node_add = {
	.name = "+" _arithType_,
	.arity    = 2,
	.evaluate = eval_add,
};

_evalArith_(eval_sub, -, n)
const stdNode node_sub = {
	.name = "-" _arithType_,
	.arity     = 2,
	.evaluate  = eval_sub,
};

_evalArith_(eval_mul, *, n)
const stdNode node_mul = {
	.name = "*" _arithType_,
	.arity    = 2,
	.evaluate = eval_mul,
};

_evalArith_(eval_div, /, n)
const stdNode node_div = {
	.name = "/" _arithType_,
	.arity    = 2,
	.evaluate = eval_div,
};


void eval_currentFrame(nodeIndex self) {
	nodes[self].output.n = curFrame;
}
const stdNode node_curFrame = {
	.name = "currentFrame num",
	.arity = 0,
	.evaluate = eval_currentFrame,
};



#define stdNodeTableLength  5
const stdNode *stdNodeTable[stdNodeTableLength] = {
	&node_add,
	&node_sub,
	&node_mul,
	&node_div,
	&node_curFrame
};




typedef struct {
	char    *name; //includes type information and parameter names
	uint32_t line; //this node was found on this line in the source file
	uint8_t  level;//elevation+fold
	int16_t  frameform;//-1 if outside not in frameform
	uint8_t  arity;//for fnDef only
} nodeInfo;
nodeInfo *nodesInfo;
