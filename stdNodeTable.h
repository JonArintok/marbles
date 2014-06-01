

const node node_numLit = {
	//.nodeID     = ID_numLit,
	.name          = "numLit",
	.inTypeString  = "",
	.outTypeString = "num",
	.arity         = 0,
	.arguments     = {},
	.evaluate      = eval_numLit,
	.output.n      = 0
};
const node node_add = {
	//.nodeID     = ID_add,
	.name          = "add",
	.inTypeString  = "num\nnum",
	.outTypeString = "num",
	.arity         = 2,
	.arguments     = {},
	.evaluate      = eval_add,
	.output.n      = 0
};
const node node_sub = {
	//.nodeID     = ID_sub,
	.name          = "sub",
	.inTypeString  = "num\nnum",
	.outTypeString = "num",
	.arity         = 2,
	.arguments     = {},
	.evaluate      = eval_sub,
	.output.n      = 0
};
const node node_mul = {
	//.nodeID     = ID_mul,
	.name          = "mul",
	.inTypeString  = "num\nnum",
	.outTypeString = "num",
	.arity         = 2,
	.arguments     = {},
	.evaluate      = eval_mul,
	.output.n      = 0
};
const node node_div = {
	//.nodeID     = ID_div,
	.name          = "div",
	.inTypeString  = "num\nnum",
	.outTypeString = "num",
	.arity         = 2,
	.arguments     = {},
	.evaluate      = eval_div,
	.output.n      = 0
};


#define stdNodeTableLength 20
node stdNodeTable[stdNodeTableLength];

int buildStdNodeTable(void) {
	
	for (int i = 0; i<nodePageSize; i++) {
		nodes[i].name = "empty";
	}
	
	stdNodeTable[0] = node_numLit;
	stdNodeTable[1] = node_add;
	stdNodeTable[2] = node_sub;
	stdNodeTable[3] = node_mul;
	stdNodeTable[4] = node_div;
	return 0;
}



