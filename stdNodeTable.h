

typedef struct {
	char     *name;
	uint8_t   arity;
	evaluator evaluate;
} stdNode;

#define _EVAL_ARITH_(name, op, type) \
void name(nodeIndex self) {\
	evalArgs(self);\
	nodes[self].output.type\
		= nodes[ nodes[self].arguments[0] ].output.type\
		op nodes[ nodes[self].arguments[1] ].output.type\
	;\
}

#define  _ARITH_TYPE_  " num\na num\nb num"

_EVAL_ARITH_(eval_add, +, n)
const stdNode node_add = {
	.name = "add" _ARITH_TYPE_,
	.arity     = 2,
	.evaluate  = eval_add,
};

_EVAL_ARITH_(eval_sub, -, n)
const stdNode node_sub = {
	.name = "sub" _ARITH_TYPE_,
	.arity     = 2,
	.evaluate  = eval_sub,
};

_EVAL_ARITH_(eval_mul, *, n)
const stdNode node_mul = {
	.name = "mul" _ARITH_TYPE_,
	.arity     = 2,
	.evaluate  = eval_mul,
};

_EVAL_ARITH_(eval_div, /, n)
const stdNode node_div = {
	.name = "div" _ARITH_TYPE_,
	.arity     = 2,
	.evaluate  = eval_div,
};

#define stdNodeTableLength  4
const stdNode *stdNodeTable[stdNodeTableLength] = {
	&node_add,
	&node_sub,
	&node_mul,
	&node_div
};

