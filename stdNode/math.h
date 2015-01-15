
_biop_(eval_add, node_add, "+", +, n)
_biop_(eval_sub, node_sub, "-", -, n)
_biop_(eval_mul, node_mul, "*", *, n)
_biop_(eval_div, node_div, "/", /, n)


outType eval_mod(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType a = output(arg0, fnCallArgs);
	outType b = output(arg1, fnCallArgs);
	outType toBeReturned;
	const int A = a.n;
	const int B = b.n;
	toBeReturned.n = A % B;
	return toBeReturned;
}
const stdNode node_mod = {
	.name = "% N1\na N1\nb N1",
	.arity = 2,
	.evaluate = eval_mod
};


outType eval_sqr(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = output(arg, fnCallArgs);
	outType toBeReturned;
	toBeReturned.n = argOut.n * argOut.n;
	return toBeReturned;
}
const stdNode node_sqr = {
	.name = "sqr N1\ninput N1",
	.arity = 1,
	.evaluate = eval_sqr
};


outType eval_abs(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType tbr = output(arg, fnCallArgs);
	if (tbr.n < 0)
		tbr.n *= -1;
	return tbr;
}
const stdNode node_abs = {
	.name = "abs N1\ninput N1",
	.arity = 1,
	.evaluate = eval_abs
};


outType eval_floor(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType tbr = output(arg, fnCallArgs);
	const int tbri = tbr.n;
	tbr.n = tbri;
	return tbr;
}
const stdNode node_floor = {
	.name = "floor N1\ninput N1",
	.arity = 1,
	.evaluate = eval_floor
};


outType eval_add4(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	nodeIndex arg3 = nodes[self].children[3];
	outType argOut0 = output(arg0, fnCallArgs);
	outType argOut1 = output(arg1, fnCallArgs);
	outType argOut2 = output(arg2, fnCallArgs);
	outType argOut3 = output(arg3, fnCallArgs);
	outType toBeReturned;
	toBeReturned.n = 
		argOut0.n +
		argOut1.n +
		argOut2.n +
		argOut3.n;
	return toBeReturned;
}
const stdNode node_add4 = {
	.name = "add4 N1\na N1\nb N1\nc N1\nd N1",
	.arity = 4,
	.evaluate = eval_add4
};

outType eval_add8(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	nodeIndex arg3 = nodes[self].children[3];
	nodeIndex arg4 = nodes[self].children[4];
	nodeIndex arg5 = nodes[self].children[5];
	nodeIndex arg6 = nodes[self].children[6];
	nodeIndex arg7 = nodes[self].children[7];
	outType argOut0 = output(arg0, fnCallArgs);
	outType argOut1 = output(arg1, fnCallArgs);
	outType argOut2 = output(arg2, fnCallArgs);
	outType argOut3 = output(arg3, fnCallArgs);
	outType argOut4 = output(arg4, fnCallArgs);
	outType argOut5 = output(arg5, fnCallArgs);
	outType argOut6 = output(arg6, fnCallArgs);
	outType argOut7 = output(arg7, fnCallArgs);
	outType toBeReturned;
	toBeReturned.n = 
		argOut0.n +
		argOut1.n +
		argOut2.n +
		argOut3.n +
		argOut4.n +
		argOut5.n +
		argOut6.n +
		argOut7.n;
	return toBeReturned;
}
const stdNode node_add8 = {
	.name = "add8 N1\na N1\nb N1\nc N1\nd N1\ne N1\nf N1\ng N1\nh N1",
	.arity = 8,
	.evaluate = eval_add8
};



outType eval_sin(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = output(arg, fnCallArgs);
	outType toBeReturned;
	toBeReturned.n = sin(tao * argOut.n);
	return toBeReturned;
}
const stdNode node_sin = {
	.name = "sin N1\ninput N1",
	.arity = 1,
	.evaluate = eval_sin
};
