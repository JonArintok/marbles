

#define _EVAL_ARITH_(name, op, typeOut, typeIn0, typeIn1) \
void name(nodeIndex self) {\
	evalArgs(self);\
	nodes[self].output.typeOut\
		= nodes[ nodes[self].arguments[0] ].output.typeIn0\
		op nodes[ nodes[self].arguments[1] ].output.typeIn1\
	;\
}

#define  _ARITH_TYPE_  " num\na num\nb num"

_EVAL_ARITH_(eval_add, +, n, n, n)
const node node_add = {
	.name = "add" _ARITH_TYPE_,
	.arity     = 2,
	.evaluate  = eval_add,
};

_EVAL_ARITH_(eval_sub, -, n, n, n)
const node node_sub = {
	.name = "sub" _ARITH_TYPE_,
	.arity     = 2,
	.evaluate  = eval_sub,
};

_EVAL_ARITH_(eval_mul, *, n, n, n)
const node node_mul = {
	.name = "mul" _ARITH_TYPE_,
	.arity     = 2,
	.evaluate  = eval_mul,
};

_EVAL_ARITH_(eval_div, /, n, n, n)
const node node_div = {
	.name = "div" _ARITH_TYPE_,
	.arity     = 2,
	.evaluate  = eval_div,
};

#define stdNodeTableLength  4
const node *stdNodeTable[stdNodeTableLength] = {
	&node_add,
	&node_sub,
	&node_mul,
	&node_div
};
