

const node node_numLit = {
	.nodeID     = dex_numLit,
	.name       = "numLit",
	.typeString = "n",
	.arity      = 0,
	.arguments  = {},
	.output.n   = 0
};
const node node_add = {
	.nodeID     = dex_add,
	.name       = "add",
	.typeString = "nnn",
	.arity      = 2,
	.arguments  = {},
	.output.n   = 0
};
const node node_sub = {
	.nodeID     = dex_sub,
	.name       = "sub",
	.typeString = "nnn",
	.arity      = 2,
	.arguments  = {},
	.output.n   = 0
};
const node node_mul = {
	.nodeID     = dex_mul,
	.name       = "mul",
	.typeString = "nnn",
	.arity      = 2,
	.arguments  = {},
	.output.n   = 0
};
const node node_div = {
	.nodeID     = dex_div,
	.name       = "div",
	.typeString = "nnn",
	.arity      = 2,
	.arguments  = {},
	.output.n   = 0
};


#define stdNodeTableLength 20
node stdNodeTable[stdNodeTableLength];

int buildStdNodeTable(void) {
	
	for (uint32_t i = 0; i<nodePageSize; i++) {
		nodes[i].name = "empty";
	}
	
	stdNodeTable[dex_numLit] = node_numLit;
	stdNodeTable[dex_add] = node_add;
	stdNodeTable[dex_sub] = node_sub;
	stdNodeTable[dex_mul] = node_mul;
	stdNodeTable[dex_div] = node_div;
	return 0;
}



