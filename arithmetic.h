


/*inline*/void getBothArgs(uint32_t self) {
	if (nodes[ nodes[self].arguments[0] ].evaluated) {
		nodes[ nodes[self].arguments[0] ].evaluate( nodes[self].arguments[0] );
	}
	if (nodes[ nodes[self].arguments[1] ].evaluated) {
		nodes[ nodes[self].arguments[1] ].evaluate( nodes[self].arguments[1] );
	}
}


void eval_add(uint32_t self) {
	getBothArgs(self);
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		+ nodes[ nodes[self].arguments[1] ].output.n
	;
}
void eval_sub(uint32_t self) {
	getBothArgs(self);
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		- nodes[ nodes[self].arguments[1] ].output.n
	;
}
void eval_mul(uint32_t self) {
	getBothArgs(self);
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		* nodes[ nodes[self].arguments[1] ].output.n
	;
}
void eval_div(uint32_t self) {
	getBothArgs(self);
	nodes[self].output.n
		= nodes[ nodes[self].arguments[0] ].output.n
		/ nodes[ nodes[self].arguments[1] ].output.n
	;
}

