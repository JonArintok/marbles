
_biop_(eval_add, node_add, "+", +, n)
_biop_(eval_sub, node_sub, "-", -, n)
_biop_(eval_mul, node_mul, "*", *, n)
_biop_(eval_div, node_div, "/", /, n)



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
