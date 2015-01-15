

outType eval_buildB4D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType widthSource  = output(arg0, fnCallArgs);
	outType heightSource = output(arg1, fnCallArgs);
	size_t newDataSize = sizeof(byte) * 4 * widthSource.n * heightSource.n;
	
	setLoadedNode(self, newDataSize);
	nodes[self].cache.B.dimenX = widthSource.n;
	nodes[self].cache.B.dimenY = heightSource.n;
	nodes[self].cache.B.dimenZ = 1;
	
	return nodes[self].cache;
}
const stdNode node_buildB4D2 = {
	.name = "buildB4D2 B4D2\nwidth N1\nheight N1",
	.arity = 2,
	.evaluate = eval_buildB4D2
};

outType eval_buildN1D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType widthSource  = output(arg0, fnCallArgs);
	outType heightSource = output(arg1, fnCallArgs);
	size_t newDataSize = sizeof(number) * widthSource.n * heightSource.n;
	
	setLoadedNode(self, newDataSize);
	nodes[self].cache.N.dimenX = widthSource.n;
	nodes[self].cache.N.dimenY = heightSource.n;
	nodes[self].cache.B.dimenZ = 1;
	
	return nodes[self].cache;
}
const stdNode node_buildN1D2 = {
	.name = "buildN1D2 N1D2\nwidth N1\nheight N1",
	.arity = 2,
	.evaluate = eval_buildN1D2
};


outType eval_fillB4D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType value  = output(arg0, fnCallArgs);
	outType source = output(arg1, fnCallArgs);
	
	outType toBeReturned = source;
	size_t newDataSize = toBeReturned.B.dataSize;
	
	if (isReadOnly(arg1)) {
		setLoadedNode(self, newDataSize);
		toBeReturned.B.data = nodes[self].cache.B.data;
	}
	
	byte *dataToBeReturned = toBeReturned.B.data;
	for (int i = 0; i < newDataSize; i += 4) {
		dataToBeReturned[i  ] = value.bt[0];
		dataToBeReturned[i+1] = value.bt[1];
		dataToBeReturned[i+2] = value.bt[2];
		dataToBeReturned[i+3] = value.bt[3];
	}
	
	return toBeReturned;
}
const stdNode node_fillB4D2 = {
	.name = "fillB4D2 B4D2\nvalue B4\nsource B4D2",
	.arity = 2,
	.evaluate = eval_fillB4D2
};


outType eval_mapB4D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType filler = output(arg0, fnCallArgs);
	outType source = output(arg1, fnCallArgs);
	
	outType toBeReturned = source;
	size_t newDataSize = toBeReturned.B.dataSize;
	
	if (isReadOnly(arg1)) {
		setLoadedNode(self, newDataSize);
		toBeReturned.B.data = nodes[self].cache.B.data;
	}
	
	//filler arguments: x, y, width, height
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = toBeReturned.B.dimenX;
	fillerCallArgs[3].n = toBeReturned.B.dimenY;
	byte *dataToBeReturned = toBeReturned.B.data;
	for (int yPos = 0; yPos < toBeReturned.B.dimenY; yPos++) {
		fillerCallArgs[1].n = yPos;
		for (int xPos = 0; xPos < toBeReturned.B.dimenX; xPos++) {
			fillerCallArgs[0].n = xPos;
			outType value = output(filler.f + 1, fillerCallArgs);
			int dataPos = (yPos * toBeReturned.B.dimenX + xPos) * 4;
			dataToBeReturned[dataPos  ] = value.bt[0];
			dataToBeReturned[dataPos+1] = value.bt[1];
			dataToBeReturned[dataPos+2] = value.bt[2];
			dataToBeReturned[dataPos+3] = value.bt[3];
		}
	}
	
	return toBeReturned;
}
const stdNode node_mapB4D2 = {
	.name = "mapB4D2 B4D2\nfiller B4 & N1 N1 N1 N1\nsource B4D2",
	.arity = 2,
	.evaluate = eval_mapB4D2
};


outType eval_mapN1D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType filler = output(arg0, fnCallArgs);
	outType source = output(arg1, fnCallArgs);
	
	outType toBeReturned = source;
	size_t newDataSize = toBeReturned.N.dataSize;
	
	if (isReadOnly(arg1)) {
		setLoadedNode(self, newDataSize);
		toBeReturned.N.data = nodes[self].cache.N.data;
	}
	
	//filler arguments: x, y, width, height
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = toBeReturned.N.dimenX;
	fillerCallArgs[3].n = toBeReturned.N.dimenY;
	number *dataToBeReturned = toBeReturned.N.data;
	for (int yPos = 0; yPos < toBeReturned.N.dimenY; yPos++) {
		fillerCallArgs[1].n = yPos;
		for (int xPos = 0; xPos < toBeReturned.N.dimenX; xPos++) {
			fillerCallArgs[0].n = xPos;
			outType value = output(filler.f + 1, fillerCallArgs);
			int dataPos = (yPos * toBeReturned.N.dimenX) + xPos;
			dataToBeReturned[dataPos] = value.n;
		}
	}
	
	return toBeReturned;
}
const stdNode node_mapN1D2 = {
	.name = "mapN1D2 N1D2\nfiller N1 & N1 N1 N1 N1\nsource N1D2",
	.arity = 2,
	.evaluate = eval_mapN1D2
};


outType eval_mapInB4D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	outType filler = output(arg0, fnCallArgs);
	outType rect   = output(arg1, fnCallArgs);
	outType source = output(arg2, fnCallArgs);
	
	outType toBeReturned = source;
	
	if (isReadOnly(arg2)) {
		setLoadedNode(self, toBeReturned.N.dataSize);
		toBeReturned.N.data = nodes[self].cache.N.data;
		memcpy(toBeReturned.N.data, source.N.data, toBeReturned.N.dataSize);
	}
	
	const number rectX  = rect.nt[0];
	const number rectY  = rect.nt[1];
	const number rectW  = rect.nt[2];
	const number rectH  = rect.nt[3];
	const number dimenX = toBeReturned.B.dimenX;
	const number dimenY = toBeReturned.B.dimenY;
	
	if (
		rectX >= dimenX   ||
		rectY >= dimenY   ||
		rectX + rectW < 0 ||
		rectY + rectH < 0 ||
		rectW < 1         ||
		rectH < 1
	) {
		return toBeReturned;
	}
	
	//filler arguments: x, y, width, height
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = rectW;
	fillerCallArgs[3].n = rectH;
	
	const int limitX = rectX+rectW < dimenX  ?  rectX+rectW  :  dimenX;
	const int limitY = rectY+rectH < dimenY  ?  rectY+rectH  :  dimenY;
	
	byte *dataToBeReturned = toBeReturned.B.data;
	for (
		int yPos  =  rectY < 0  ?  0  :  rectY; 
		yPos < limitY; 
		yPos++
	) {
		fillerCallArgs[1].n = yPos - rectY;
		for (
			int xPos  =  rectX < 0  ?  0  :  rectX; 
			xPos < limitX; 
			xPos++
		) {
			fillerCallArgs[0].n = xPos - rectX;
			outType value = output(filler.f + 1, fillerCallArgs);
			int dataPos = (yPos * dimenX + xPos) * 4;
			dataToBeReturned[dataPos  ] = value.bt[0];
			dataToBeReturned[dataPos+1] = value.bt[1];
			dataToBeReturned[dataPos+2] = value.bt[2];
			dataToBeReturned[dataPos+3] = value.bt[3];
		}
	}
	
	return toBeReturned;
}
const stdNode node_mapInB4D2 = {
	.name = "mapInB4D2 B4D2\nfiller B4 & N1 N1 N1 N1\nrect N4\nsource B4D2",
	.arity = 3,
	.evaluate = eval_mapInB4D2
};



outType eval_mapInN1D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	outType filler = output(arg0, fnCallArgs);
	outType rect   = output(arg1, fnCallArgs);
	outType source = output(arg2, fnCallArgs);
	
	outType toBeReturned = source;
	
	if (isReadOnly(arg2)) {
		setLoadedNode(self, toBeReturned.N.dataSize);
		toBeReturned.N.data = nodes[self].cache.N.data;
		memcpy(toBeReturned.N.data, source.N.data, toBeReturned.N.dataSize);
	}
	
	const number rectX  = rect.nt[0];
	const number rectY  = rect.nt[1];
	const number rectW  = rect.nt[2];
	const number rectH  = rect.nt[3];
	const number dimenX = toBeReturned.B.dimenX;
	const number dimenY = toBeReturned.B.dimenY;
	
	if (
		rectX >= dimenX   ||
		rectY >= dimenY   ||
		rectX + rectW < 0 ||
		rectY + rectH < 0 ||
		rectW < 1         ||
		rectH < 1
	) {
		return toBeReturned;
	}
	
	//filler arguments: x, y, width, height
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = rectW;
	fillerCallArgs[3].n = rectH;
	
	const int limitX = rectX+rectW < dimenX  ?  rectX+rectW  :  dimenX;
	const int limitY = rectY+rectH < dimenY  ?  rectY+rectH  :  dimenY;
	
	number *dataToBeReturned = toBeReturned.N.data;
	for (
		int yPos = rectY < 0  ?  0  :  rectY; 
		yPos < limitY; 
		yPos++
	) {
		fillerCallArgs[1].n = yPos - rectY;
		for (
			int xPos = rectX < 0  ?  0  :  rectX; 
			xPos < limitX; 
			xPos++
		) {
			fillerCallArgs[0].n = xPos - rectX;
			outType value = output(filler.f + 1, fillerCallArgs);
			int dataPos = (yPos * toBeReturned.N.dimenX) + xPos;
			dataToBeReturned[dataPos] = value.n;
		}
	}
	
	return toBeReturned;
}
const stdNode node_mapInN1D2 = {
	.name = "mapInN1D2 N1D2\nfiller N1 & N1 N1 N1 N1\nrect N4\nsource N1D2",
	.arity = 3,
	.evaluate = eval_mapInN1D2
};


outType eval_randomBoolFillN1D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	outType source = output(arg0, fnCallArgs);
	
	outType toBeReturned = source;
	size_t newDataSize = toBeReturned.N.dataSize;
	
	if (isReadOnly(arg0)) {
		setLoadedNode(self, newDataSize);
		toBeReturned.N.data = nodes[self].cache.N.data;
	}
	
	number *dataToBeReturned = toBeReturned.N.data;
	int elements = toBeReturned.N.dimenX * toBeReturned.N.dimenY;
	for (int i = 0; i < elements; i++)
		dataToBeReturned[i] = rand()%2;
	
	return toBeReturned;
}
const stdNode node_randomBoolFillN1D2 = {
	.name = "randomBoolFillN1D2 N1D2\nsource N1D2",
	.arity = 1,
	.evaluate = eval_randomBoolFillN1D2
};



outType eval_fromN1D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	outType x      = output(arg0, fnCallArgs);
	outType y      = output(arg1, fnCallArgs);
	outType source = output(arg2, fnCallArgs);
	outType toBeReturned;
	if      (x.n >= source.N.dimenX  ||  x.n < 0)
		toBeReturned.n = 0;
	else if (y.n >= source.N.dimenY  ||  y.n < 0)
		toBeReturned.n = 0;
	else {
		int i = (y.n * source.N.dimenX) + x.n;
		toBeReturned.n = source.N.data[i];
	}
	return toBeReturned;
}
const stdNode node_fromN1D2 = {
	.name = "fromN1D2 N1\nx N1\ny N1\nsource N1D2",
	.arity = 3,
	.evaluate = eval_fromN1D2
};



outType eval_widthOf(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = output(arg, fnCallArgs);
	outType toBeReturned;
	toBeReturned.n = argOut.B.dimenX;
	return toBeReturned;
}
const stdNode node_widthOf = {
	.name = "widthOf N1\narray N3",
	.arity = 1,
	.evaluate = eval_widthOf
};

outType eval_heightOf(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = output(arg, fnCallArgs);
	outType toBeReturned;
	toBeReturned.n = argOut.B.dimenY;
	return toBeReturned;
}
const stdNode node_heightOf = {
	.name = "heightOf N1\narray N3",
	.arity = 1,
	.evaluate = eval_heightOf
};


