

const node node_numLit = {
	.name       = "numLit",
	.typeString = "n",
	.evaluated  = false,
	.arity      = 0,
	.arguments  = {},
	.evaluate   = NULL,
	.output.n   = 0
};


const node node_add = {
	.name       = "add",
	.typeString = "nnn",
	.evaluated  = true,
	.arity      = 2,
	.arguments  = {},
	.evaluate   = eval_add,
	.output.n   = 0
};
const node node_sub = {
	.name       = "sub",
	.typeString = "nnn",
	.evaluated  = true,
	.arity      = 2,
	.arguments  = {},
	.evaluate   = eval_sub,
	.output.n   = 0
};
const node node_mul = {
	.name       = "mul",
	.typeString = "nnn",
	.evaluated  = true,
	.arity      = 2,
	.arguments  = {},
	.evaluate   = eval_mul,
	.output.n   = 0
};
const node node_div = {
	.name       = "div",
	.typeString = "nnn",
	.evaluated  = true,
	.arity      = 2,
	.arguments  = {},
	.evaluate   = eval_div,
	.output.n   = 0
};


#define stdNodeTableLength 4
node stdNodeTable[stdNodeTableLength];

int buildStdNodeTable(void) {
	
	uint i = 0;
	for (; i<nodePageSize; i++) {
		nodes[i].name = "empty";
	}
	
	stdNodeTable[0] = node_add;
	stdNodeTable[1] = node_sub;
	stdNodeTable[2] = node_mul;
	stdNodeTable[3] = node_div;
	
	return 0;
}



