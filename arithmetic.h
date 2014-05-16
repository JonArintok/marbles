


inline void getBothArgs(node *self) {
	if (self->arguments[0].evaluated) {
		self->arguments[0].evaluate( &(self->arguments[0]) );
	}
	if (self->arguments[1].evaluated) {
		self->arguments[1].evaluate( &(self->arguments[1]) );
	}
}


void eval_add(node *self) {
	getBothArgs(self);
	self->output.n
		= self->arguments[0].output.n
		+ self->arguments[1].output.n
	;
}
void eval_sub(node *self) {
	getBothArgs(self);
	self->output.n
		= self->arguments[0].output.n
		- self->arguments[1].output.n
	;
}
void eval_mul(node *self) {
	getBothArgs(self);
	self->output.n
		= self->arguments[0].output.n
		* self->arguments[1].output.n
	;
}
void eval_div(node *self) {
	getBothArgs(self);
	self->output.n
		= self->arguments[0].output.n
		/ self->arguments[1].output.n
	;
}

