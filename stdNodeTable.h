

void eval_numLit(nodeIndex self) {}
const node node_numLit = {
	.name =
		"numLit\n"
		"\tnum",
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
		"\tnum",
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
		"\tnum",
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
		"\tnum",
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
		"\tnum",
	.arity         = 2,
	.arguments     = {},
	.evaluate      = eval_div,
	.output.n      = 0
};

#define stdNodeTableLength  5
const node *stdNodeTable[stdNodeTableLength] = {
	&node_numLit,
	&node_add,
	&node_sub,
	&node_mul,
	&node_div
};



