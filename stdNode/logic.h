
outType eval_if(_evalargs_) {
	nodeIndex cond = nodes[self].children[0];
	outType condOut = _output_(cond, fnCallArgs)
	nodeIndex selection;
	if (condOut.n)
		selection = nodes[self].children[1];
	else
		selection = nodes[self].children[2];
	return _output_(selection, fnCallArgs)
}
const stdNode node_if = {
	.name = "? match\ncondition N1\nifTrue match\nelse match",
	.arity = 3,
	.evaluate = eval_if
};



_biop_(eval_equal,       node_equal,        "=",  ==, n)
_biop_(eval_greaterThan, node_greaterThan,  ">",  >,  n)
_biop_(eval_lessThan,    node_lessThan,     "<",  <,  n)
_biop_(eval_notEqual,    node_notEqual,    "!=",  !=, n)
_biop_(eval_greaterThanOrEqual, node_greaterThanOrEqual, ">=", >=, n)
_biop_(eval_lessThanOrEqual,    node_lessThanOrEqual,    "<=", <=, n)
_biop_(eval_both,   node_both,   "both",   &&, n)
_biop_(eval_either, node_either, "either", ||, n)


outType eval_not(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = _output_(arg, fnCallArgs)
	outType toBeReturned;
	if (argOut.n)
		toBeReturned.n = 0;
	else
		toBeReturned.n = 1;
	return toBeReturned;
}
const stdNode node_not = {
	.name = "! N1\na N1",
	.arity = 1,
	.evaluate = eval_not
};


