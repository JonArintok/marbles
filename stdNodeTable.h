

#define _biop_(eval_name, node_name, title, op, type) \
outType eval_name(nodeIndex self, outType fnCallArgs[maxChildren]) {\
	nodeIndex arg0 = nodes[self].children[0];\
	nodeIndex arg1 = nodes[self].children[1];\
	outType a = _output_(arg0, fnCallArgs)\
	outType b = _output_(arg1, fnCallArgs)\
	outType toBeReturned;\
	toBeReturned.type = a.type op b.type;\
	return toBeReturned;\
}\
const stdNode node_name = {\
	.name = title " num\na num\nb num",\
	.arity    = 2,\
	.evaluate = eval_name\
};

_biop_(eval_add, node_add, "+", +, n)
_biop_(eval_sub, node_sub, "-", -, n)
_biop_(eval_mul, node_mul, "*", *, n)
_biop_(eval_div, node_div, "/", /, n)
_biop_(eval_equal,          node_equal,           "=", ==, n)
_biop_(eval_lessThan,       node_lessThan,        "<",  <, n)
_biop_(eval_greaterThan,    node_greaterThan,     ">",  >, n)
_biop_(eval_notEqual,       node_notEqual,       "!=", !=, n)
_biop_(eval_notLessThan,    node_notLessThan,    "!<", >=, n)
_biop_(eval_notGreaterThan, node_notGreaterThan, "!>", <=, n)
_biop_(eval_both,           node_both,       "both",   &&, n)
_biop_(eval_either,         node_either,     "either", ||, n)


outType eval_if(nodeIndex self, outType fnCallArgs[maxChildren]) {
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
	.name = "? match\ncondition num\nifTrue match\nelse match",
	.arity = 3,
	.evaluate = eval_if,
};

outType eval_not(nodeIndex self, outType fnCallArgs[maxChildren]) {
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
	.name = "! num\na num",
	.arity = 1,
	.evaluate = eval_not,
};

outType eval_currentFrame(nodeIndex self, outType fnCallArgs[maxChildren]) {
	outType toBeReturned;
	toBeReturned.n = curFrame;
	return toBeReturned;
}
const stdNode node_curFrame = {
	.name = "currentFrame num",
	.arity = 0,
	.evaluate = eval_currentFrame,
};





const stdNode *stdNodeTable[stdNodeTableLength] = {
	&node_add,
	&node_sub,
	&node_mul,
	&node_div,
	&node_equal,
	&node_lessThan,
	&node_greaterThan,
	&node_notEqual,
	&node_notLessThan,
	&node_notGreaterThan,
	&node_both,
	&node_either,
	&node_if,
	&node_not,
	&node_curFrame
};

