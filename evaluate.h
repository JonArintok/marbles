

void evaluateBranch(nodeIndex toBeEvaluated) {
	for (int i=0; i < nodes[toBeEvaluated].arity; i++)
		evaluateBranch( nodes[toBeEvaluated].arguments[i] );
	nodes[toBeEvaluated].evaluate(toBeEvaluated);
}


void eval_numLit(nodeIndex self) {}

void eval_add(nodeIndex self) {
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		+ nodes[ nodes[self].arguments[1] ].output.n
	;
}
void eval_sub(nodeIndex self) {
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		- nodes[ nodes[self].arguments[1] ].output.n
	;
}
void eval_mul(nodeIndex self) {
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		* nodes[ nodes[self].arguments[1] ].output.n
	;
}
void eval_div(nodeIndex self) {
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		/ nodes[ nodes[self].arguments[1] ].output.n
	;
}

/*


typedef struct stackthing {
	uint32_t  fnDef;
	uint32_t  callSource;
};

#define stacksize
stackthing stack[stacksize]
uint32_t stackPos;



eval_varCall
	//nothing to do, value only ever changes between frames or on edit


eval_fnCall
	
	push the stack
	if there are arguments, evaluate them
	evaluate the function
	pop the stack
	



eval_argCall
	
	refer to the stack
	



*/

