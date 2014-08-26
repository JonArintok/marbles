

typedef struct {
	char     *name;
	uint8_t   arity;
	evaluator evaluate;
} stdNode;

#define _biop_(eval_name, node_name, title, op, type) \
void eval_name(nodeIndex self) {\
	nodeIndex arg0 = nodes[self].children[0];\
	nodeIndex arg1 = nodes[self].children[1];\
	_evaluateNode_(arg0)\
	_evaluateNode_(arg1)\
	nodes[self].output.type = \
		nodes[arg0].output.type op nodes[arg1].output.type;\
}\
const stdNode node_name = {\
	.name = title " num\na num\nb num",\
	.arity    = 2,\
	.evaluate = eval_name\
};

_biop_(eval_add, node_add, "+", +, n)
_biop_(eval_sub, node_sub, "-", -, n)
_biop_(eval_mul, node_mul, "*", *, n)
_biop_(eval_div, node_div, "/", /, n)
_biop_(eval_equal,          node_equal,           "=", ==, n)
_biop_(eval_lessThan,       node_lessThan,        "<",  <, n)
_biop_(eval_greaterThan,    node_greaterThan,     ">",  >, n)
_biop_(eval_notEqual,       node_notEqual,       "!=", !=, n)
_biop_(eval_notLessThan,    node_notLessThan,    "!<", >=, n)
_biop_(eval_notGreaterThan, node_notGreaterThan, "!>", <=, n)

void eval_if(nodeIndex self) {
	nodeIndex cond = nodes[self].children[0];
	nodeIndex selection;
	_evaluateNode_(cond)
	if (nodes[cond].output.n)
		selection = nodes[self].children[1];
	else
		selection = nodes[self].children[2];
	_evaluateNode_(selection)
	nodes[self].output = nodes[selection].output;
}
const stdNode node_if = {
	.name = "? num\ncondition num\nifTrue num\nelse num",
	.arity = 3,
	.evaluate = eval_if,
};

void eval_not(nodeIndex self) {
	nodeIndex arg = nodes[self].children[0];
	_evaluateNode_(arg)
	nodes[self].output.n = nodes[arg].output.n ? 0 : 1;
}
const stdNode node_not = {
	.name = "! num\na num",
	.arity = 1,
	.evaluate = eval_not,
};

void eval_currentFrame(nodeIndex self) {
	nodes[self].output.n = curFrame;
}
const stdNode node_curFrame = {
	.name = "currentFrame num",
	.arity = 0,
	.evaluate = eval_currentFrame,
};



#define stdNodeTableLength  13
const stdNode *stdNodeTable[stdNodeTableLength] = {
	&node_add,
	&node_sub,
	&node_mul,
	&node_div,
	&node_equal,
	&node_lessThan,
	&node_greaterThan,
	&node_notEqual,
	&node_notLessThan,
	&node_notGreaterThan,
	&node_if,
	&node_not,
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
