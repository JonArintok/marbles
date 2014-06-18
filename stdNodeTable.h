

void eval_numLit(nodeIndex self) {}
const node node_numLit = {
	.name =
		"numLit\n"
		"\tis  num",
	.arity         = 0,
	.arguments     = {},
	.evaluate      = eval_numLit,
	.output.n      = 0
};


void eval_add(nodeIndex self) {
	evalArgs(self);
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		+ nodes[ nodes[self].arguments[1] ].output.n
	;
}
const node node_add = {
	.name =
		"add\n"
		"\tnum  a\n"
		"\tnum  b\n"
		"\tis  num",
	.arity         = 2,
	.arguments     = {},
	.evaluate      = eval_add,
	.output.n      = 0
};


void eval_sub(nodeIndex self) {
	evalArgs(self);
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		- nodes[ nodes[self].arguments[1] ].output.n
	;
}
const node node_sub = {
	.name = 
		"sub\n"
		"\tnum  a\n"
		"\tnum  b\n"
		"\tis  num",
	.arity         = 2,
	.arguments     = {},
	.evaluate      = eval_sub,
	.output.n      = 0
};


void eval_mul(nodeIndex self) {
	evalArgs(self);
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		* nodes[ nodes[self].arguments[1] ].output.n
	;
}
const node node_mul = {
	.name =
		"mul\n"
		"\tnum  a\n"
		"\tnum  b\n"
		"\tis  num",
	.arity         = 2,
	.arguments     = {},
	.evaluate      = eval_mul,
	.output.n      = 0
};


void eval_div(nodeIndex self) {
	evalArgs(self);
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		/ nodes[ nodes[self].arguments[1] ].output.n
	;
}
const node node_div = {
	.name =
		"div\n"
		"\tnum  a\n"
		"\tnum  b\n"
		"\tis  num",
	.arity         = 2,
	.arguments     = {},
	.evaluate      = eval_div,
	.output.n      = 0
};

#define nodeDefPageSize  100
int nodeDefTableLength = 5;
const node *nodeDefTable[nodeDefPageSize] = {
	&node_numLit,
	&node_add,
	&node_sub,
	&node_mul,
	&node_div
};

//#define  userNodeTableLength 100
//node  *usrNodeTable[userNodeTableLength]



