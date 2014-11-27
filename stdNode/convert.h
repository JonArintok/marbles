
outType eval_byteFrom01(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = _output_(arg, fnCallArgs)
	outType toBeReturned;
	toBeReturned.b = argOut.n * 255;
	return toBeReturned;
}
const stdNode node_byteFrom01 = {
	.name = "byteFrom01 byte\ninput num",
	.arity = 1,
	.evaluate = eval_byteFrom01
};



outType eval_byte4From01(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	nodeIndex arg3 = nodes[self].children[3];
	outType argOut0 = _output_(arg0, fnCallArgs)
	outType argOut1 = _output_(arg1, fnCallArgs)
	outType argOut2 = _output_(arg2, fnCallArgs)
	outType argOut3 = _output_(arg3, fnCallArgs)
	outType toBeReturned;
	toBeReturned.b4[0] = argOut0.n * 255;
	toBeReturned.b4[1] = argOut1.n * 255;
	toBeReturned.b4[2] = argOut2.n * 255;
	toBeReturned.b4[3] = argOut3.n * 255;
	return toBeReturned;
}
const stdNode node_byte4From01 = {
	.name = "byte4From01 byte4\nb0 num\nb1 num\nb2 num\nb3 num",
	.arity = 4,
	.evaluate = eval_byte4From01
};


