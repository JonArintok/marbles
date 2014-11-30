
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
