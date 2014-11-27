

outType eval_num4(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	nodeIndex arg3 = nodes[self].children[3];
	outType argOut0 = _output_(arg0, fnCallArgs)
	outType argOut1 = _output_(arg1, fnCallArgs)
	outType argOut2 = _output_(arg2, fnCallArgs)
	outType argOut3 = _output_(arg3, fnCallArgs)
	outType toBeReturned;
	toBeReturned.n4[0] = argOut0.n;
	toBeReturned.n4[1] = argOut1.n;
	toBeReturned.n4[2] = argOut2.n;
	toBeReturned.n4[3] = argOut3.n;
	return toBeReturned;
}
const stdNode node_num4 = {
	.name = "num4 num4\nn0 num\nn1 num\nn2 num\nn3 num",
	.arity = 4,
	.evaluate = eval_num4
};





outType eval_fillByte4(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = _output_(arg, fnCallArgs)
	outType toBeReturned;
	toBeReturned.b4[0] = argOut.b;
	toBeReturned.b4[1] = argOut.b;
	toBeReturned.b4[2] = argOut.b;
	toBeReturned.b4[3] = argOut.b;
	return toBeReturned;
}
const stdNode node_fillByte4 = {
	.name = "fillByte4 byte4\nvalue byte",
	.arity = 1,
	.evaluate = eval_fillByte4
};

