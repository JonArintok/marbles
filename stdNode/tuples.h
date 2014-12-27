

outType eval_N4(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	nodeIndex arg3 = nodes[self].children[3];
	outType argOut0 = _output_(arg0, fnCallArgs)
	outType argOut1 = _output_(arg1, fnCallArgs)
	outType argOut2 = _output_(arg2, fnCallArgs)
	outType argOut3 = _output_(arg3, fnCallArgs)
	outType toBeReturned;
	toBeReturned.nt[0] = argOut0.n;
	toBeReturned.nt[1] = argOut1.n;
	toBeReturned.nt[2] = argOut2.n;
	toBeReturned.nt[3] = argOut3.n;
	return toBeReturned;
}
const stdNode node_N4 = {
	.name = "N4 N4\na N1\nb N1\nc N1\nf N1",
	.arity = 4,
	.evaluate = eval_N4
};





outType eval_fillB4(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = _output_(arg, fnCallArgs)
	outType toBeReturned;
	toBeReturned.bt[0] = argOut.b;
	toBeReturned.bt[1] = argOut.b;
	toBeReturned.bt[2] = argOut.b;
	toBeReturned.bt[3] = argOut.b;
	return toBeReturned;
}
const stdNode node_fillB4 = {
	.name = "fillB4 B4\nvalue B1",
	.arity = 1,
	.evaluate = eval_fillB4
};

