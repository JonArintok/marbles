
outType eval_B1from01(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = output(arg, fnCallArgs);
	outType toBeReturned;
	toBeReturned.b = argOut.n * 255;
	return toBeReturned;
}
const stdNode node_B1from01 = {
	.name = "B1from01 B1\ninput N1",
	.arity = 1,
	.evaluate = eval_B1from01
};



outType eval_B4from01(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	nodeIndex arg3 = nodes[self].children[3];
	outType argOut0 = output(arg0, fnCallArgs);
	outType argOut1 = output(arg1, fnCallArgs);
	outType argOut2 = output(arg2, fnCallArgs);
	outType argOut3 = output(arg3, fnCallArgs);
	outType toBeReturned;
	toBeReturned.bt[0] = argOut0.n * 255;
	toBeReturned.bt[1] = argOut1.n * 255;
	toBeReturned.bt[2] = argOut2.n * 255;
	toBeReturned.bt[3] = argOut3.n * 255;
	return toBeReturned;
}
const stdNode node_B4from01 = {
	.name = "B4from01 B4\na N1\nb N1\nc N1\nd N1",
	.arity = 4,
	.evaluate = eval_B4from01
};


