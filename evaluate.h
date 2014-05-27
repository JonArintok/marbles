
enum {
	dex_numLit,
	dex_add,
	dex_sub,
	dex_mul,
	dex_div
};

void evaluate(uint32_t toBeEvaluated) {
	//if toBeEvaluated is > stdNodeTableLength, 
	//then toBeEvaluated-stdNodeTableLength is the index of
	//the user defined function in usrNodeTable to be evaluated.
	//else...
	for (uint32_t i=0; i < nodes[toBeEvaluated].arity; i++) {
		evaluate( nodes[toBeEvaluated].arguments[i] );
	}
	switch (nodes[toBeEvaluated].nodeID) {
		case dex_numLit:
			break;
		case dex_add:
			nodes[toBeEvaluated].output.n
				= nodes[ nodes[toBeEvaluated].arguments[0] ].output.n
				+ nodes[ nodes[toBeEvaluated].arguments[1] ].output.n
			;
			break;
		case dex_sub:
			nodes[toBeEvaluated].output.n
				= nodes[ nodes[toBeEvaluated].arguments[0] ].output.n
				- nodes[ nodes[toBeEvaluated].arguments[1] ].output.n
			;
			break;
		case dex_mul:
			nodes[toBeEvaluated].output.n
				= nodes[ nodes[toBeEvaluated].arguments[0] ].output.n
				* nodes[ nodes[toBeEvaluated].arguments[1] ].output.n
			;
			break;
		case dex_div:
			nodes[toBeEvaluated].output.n
				= nodes[ nodes[toBeEvaluated].arguments[0] ].output.n
				/ nodes[ nodes[toBeEvaluated].arguments[1] ].output.n
			;
			break;
		default:
			printf("evaluate defaulted! toBeEvaluated: %d\n", toBeEvaluated);
	}
}

