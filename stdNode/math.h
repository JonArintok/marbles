
_biop_(eval_add, node_add, "+", +, n)
_biop_(eval_sub, node_sub, "-", -, n)
_biop_(eval_mul, node_mul, "*", *, n)
_biop_(eval_div, node_div, "/", /, n)


outType eval_mod(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType a = _output_(arg0, fnCallArgs)
	outType b = _output_(arg1, fnCallArgs)
	outType toBeReturned;
	toBeReturned.n = (int)a.n % (int)b.n;
	return toBeReturned;
}
const stdNode node_mod = {
	.name = "% num\na num\nb num",
	.arity = 2,
	.evaluate = eval_mod
};


outType eval_sqr(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = _output_(arg, fnCallArgs)
	outType toBeReturned;
	toBeReturned.n = argOut.n * argOut.n;
	return toBeReturned;
}
const stdNode node_sqr = {
	.name = "sqr num\ninput num",
	.arity = 1,
	.evaluate = eval_sqr
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
	outType argOut0 = _output_(arg0, fnCallArgs)
	outType argOut1 = _output_(arg1, fnCallArgs)
	outType argOut2 = _output_(arg2, fnCallArgs)
	outType argOut3 = _output_(arg3, fnCallArgs)
	outType argOut4 = _output_(arg4, fnCallArgs)
	outType argOut5 = _output_(arg5, fnCallArgs)
	outType argOut6 = _output_(arg6, fnCallArgs)
	outType argOut7 = _output_(arg7, fnCallArgs)
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
	.name = "add8 num\na num\nb num\nc num\nd num\ne num\nf num\ng num\nh num",
	.arity = 8,
	.evaluate = eval_add8
};




outType eval_sin(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = _output_(arg, fnCallArgs)
	outType toBeReturned;
	toBeReturned.n = sin(tao * argOut.n);
	return toBeReturned;
}
const stdNode node_sin = {
	.name = "sin num\ninput num",
	.arity = 1,
	.evaluate = eval_sin
};
