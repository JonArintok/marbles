

outType eval_buildByte4D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType widthSource  = _output_(arg0, fnCallArgs)
	outType heightSource = _output_(arg1, fnCallArgs)
	size_t newDataSize = sizeof(byte) * 4 * widthSource.n * heightSource.n;
	
	if (!nodes[self].cache.B.data) {
		printf("mallocing data from node %i, in eval_buildByte4D2\n", self);
		nodes[self].cache.B.data = malloc(newDataSize);
		addLoadedNode(self);
	}
	else if (nodes[self].cache.B.dataSize < newDataSize) {
		printf("reallocing data from node %i, in eval_buildByte4D2\n", self);
		nodes[self].cache.B.data = 
			realloc(nodes[self].cache.B.data, newDataSize);
	}
	nodes[self].cache.B.dataSize = newDataSize;
	nodes[self].cache.B.dimenX = widthSource.n;
	nodes[self].cache.B.dimenY = heightSource.n;
	
	return nodes[self].cache;
}
const stdNode node_buildByte4D2 = {
	.name = "buildByte4D2 byte4D2\nwidth num\nheight num",
	.arity = 2,
	.evaluate = eval_buildByte4D2
};

outType eval_buildNumD2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType widthSource  = _output_(arg0, fnCallArgs)
	outType heightSource = _output_(arg1, fnCallArgs)
	size_t newDataSize = sizeof(number) * widthSource.n * heightSource.n;
	
	if (!nodes[self].cache.N.data) {
		printf("mallocing data from node %i, in eval_buildNumD2\n", self);
		nodes[self].cache.N.data = malloc(newDataSize);
		addLoadedNode(self);
	}
	else if (nodes[self].cache.N.dataSize < newDataSize) {
		printf("reallocing data from node %i, in eval_buildNumD2\n", self);
		nodes[self].cache.N.data = 
			realloc(nodes[self].cache.N.data, newDataSize);
	}
	nodes[self].cache.N.dataSize = newDataSize;
	nodes[self].cache.N.dimenX = widthSource.n;
	nodes[self].cache.N.dimenY = heightSource.n;
	
	return nodes[self].cache;
}
const stdNode node_buildNumD2 = {
	.name = "buildNumD2 numD2\nwidth num\nheight num",
	.arity = 2,
	.evaluate = eval_buildNumD2
};


outType eval_fillByte4D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType value  = _output_(arg0, fnCallArgs);
	outType source = _output_(arg1, fnCallArgs);
	
	outType toBeReturned = source;
	size_t newDataSize = toBeReturned.B.dataSize;
	
	if (isReadOnly(arg1)) {
		if (!nodes[self].cache.B.data) {
			printf("mallocing data from node %i, in eval_fillByte4D2\n", self);
			nodes[self].cache.B.data = malloc(newDataSize);
			addLoadedNode(self);
		}
		else if (nodes[self].cache.B.dataSize < newDataSize) {
			printf("reallocing data from node %i, in eval_fillByte4D2\n", self);
			nodes[self].cache.B.data = 
				realloc(nodes[self].cache.B.data, newDataSize);
		}
		nodes[self].cache.B.dataSize = newDataSize;
		toBeReturned.B.data = nodes[self].cache.B.data;
	}
	
	byte *dataToBeReturned = toBeReturned.B.data;
	for (int i = 0; i < newDataSize; i += 4) {
		dataToBeReturned[i  ] = value.b4[0];
		dataToBeReturned[i+1] = value.b4[1];
		dataToBeReturned[i+2] = value.b4[2];
		dataToBeReturned[i+3] = value.b4[3];
	}
	
	return toBeReturned;
}
const stdNode node_fillByte4D2 = {
	.name = "fillByte4D2 byte4D2\nvalue byte4\nsource byte4D2",
	.arity = 2,
	.evaluate = eval_fillByte4D2
};



outType eval_dynamicFillByte4D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType filler = _output_(arg0, fnCallArgs);
	outType source = _output_(arg1, fnCallArgs);
	
	outType toBeReturned = source;
	size_t newDataSize = toBeReturned.B.dataSize;
	
	if (isReadOnly(arg1)) {
		if (!nodes[self].cache.B.data) {
			printf("mallocing data from node %i, in eval_dynamicFillByte4D2\n", self);
			nodes[self].cache.B.data = malloc(newDataSize);
			addLoadedNode(self);
		}
		else if (nodes[self].cache.B.dataSize < newDataSize) {
			printf("reallocing data from node %i, in eval_dynamicFillByte4D2\n", self);
			nodes[self].cache.B.data = 
				realloc(nodes[self].cache.B.data, newDataSize);
		}
		nodes[self].cache.B.dataSize = newDataSize;
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
			outType value = _output_(filler.f + 1, fillerCallArgs)
			int dataPos = (yPos * toBeReturned.B.dimenX + xPos) * 4;
			dataToBeReturned[dataPos  ] = value.b4[0];
			dataToBeReturned[dataPos+1] = value.b4[1];
			dataToBeReturned[dataPos+2] = value.b4[2];
			dataToBeReturned[dataPos+3] = value.b4[3];
		}
	}
	
	return toBeReturned;
}
const stdNode node_dynamicFillByte4D2 = {
	.name = "dynamicFillByte4D2 byte4D2\nfiller byte4 & num num num num\nsource byte4D2",
	.arity = 2,
	.evaluate = eval_dynamicFillByte4D2
};


outType eval_dynamicFillNumD2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType filler = _output_(arg0, fnCallArgs);
	outType source = _output_(arg1, fnCallArgs);
	
	outType toBeReturned = source;
	size_t newDataSize = toBeReturned.N.dataSize;
	
	if (isReadOnly(arg1)) {
		if (!nodes[self].cache.N.data) {
			printf("mallocing data from node %i, in eval_dynamicFillNumD2\n", self);
			nodes[self].cache.N.data = malloc(newDataSize);
			addLoadedNode(self);
		}
		else if (nodes[self].cache.N.dataSize < newDataSize) {
			printf("reallocing data from node %i, in eval_dynamicFillNumD2\n", self);
			nodes[self].cache.N.data = 
				realloc(nodes[self].cache.N.data, newDataSize);
		}
		nodes[self].cache.N.dataSize = newDataSize;
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
			outType value = _output_(filler.f + 1, fillerCallArgs)
			int dataPos = (yPos * toBeReturned.N.dimenX) + xPos;
			dataToBeReturned[dataPos] = value.n;
		}
	}
	
	return toBeReturned;
}
const stdNode node_dynamicFillNumD2 = {
	.name = "dynamicFillNumD2 numD2\nfiller num & num num num num\nsource numD2",
	.arity = 2,
	.evaluate = eval_dynamicFillNumD2
};



outType eval_limitedDynamicFillByte4D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	outType filler = _output_(arg0, fnCallArgs);
	outType rect   = _output_(arg1, fnCallArgs);
	outType source = _output_(arg2, fnCallArgs);
	
	outType toBeReturned = source;
	size_t newDataSize = toBeReturned.B.dataSize;
	
	if (isReadOnly(arg2)) {
		if (!nodes[self].cache.B.data) {
			printf("mallocing data from node %i, in eval_limitedDynamicFillByte4D2\n", self);
			nodes[self].cache.B.data = malloc(newDataSize);
			addLoadedNode(self);
		}
		else if (nodes[self].cache.B.dataSize < newDataSize) {
			printf("reallocing data from node %i, in eval_limitedDynamicFillByte4D2\n", self);
			nodes[self].cache.B.data = 
				realloc(nodes[self].cache.B.data, newDataSize);
		}
		nodes[self].cache.B.dataSize = newDataSize;
		toBeReturned.B.data = nodes[self].cache.B.data;
		memcpy(toBeReturned.B.data, source.B.data, newDataSize);
	}
	
	double rectX = rect.n4[0];
	double rectY = rect.n4[1];
	double rectW = rect.n4[2];
	double rectH = rect.n4[3];
	
	//clip the rectangle to prevent out-of-bounds access
	if (
		rectW < 1 || 
		rectX > toBeReturned.B.dimenX ||
		rectY > toBeReturned.B.dimenY
	) {
		return toBeReturned;
	}
	if (rectX < 0)
		rectX = 0;
	if (rectY < 0)
		rectY = 0;
	if (rectH < 1)
		rectH = 1;
	if (rectX + rectW  >  toBeReturned.B.dimenX)
		rectW = toBeReturned.B.dimenX - rectX;
	if (rectY + rectH  >  toBeReturned.B.dimenY)
		rectH = toBeReturned.B.dimenY - rectY;
	
	//filler arguments: x, y, width, height
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = rectW;
	fillerCallArgs[3].n = rectH;
	byte *dataToBeReturned = toBeReturned.B.data;
	for (int yPos = rectY; yPos  <  rectY + rectH; yPos++) {
		fillerCallArgs[1].n = yPos - rectY;
		for (int xPos = rectX; xPos  <  rectX + rectW; xPos++) {
			fillerCallArgs[0].n = xPos - rectX;
			outType value = _output_(filler.f + 1, fillerCallArgs)
			int dataPos = (yPos * toBeReturned.B.dimenX + xPos) * 4;
			dataToBeReturned[dataPos  ] = value.b4[0];
			dataToBeReturned[dataPos+1] = value.b4[1];
			dataToBeReturned[dataPos+2] = value.b4[2];
			dataToBeReturned[dataPos+3] = value.b4[3];
		}
	}
	
	return toBeReturned;
}
const stdNode node_limitedDynamicFillByte4D2 = {
	.name = "limitedDynamicFillByte4D2 byte4D2\nfiller byte4 & num num num num\nrect num4\nsource byte4D2",
	.arity = 3,
	.evaluate = eval_limitedDynamicFillByte4D2
};



outType eval_limitedDynamicFillNumD2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	outType filler = _output_(arg0, fnCallArgs);
	outType rect   = _output_(arg1, fnCallArgs);
	outType source = _output_(arg2, fnCallArgs);
	
	outType toBeReturned = source;
	size_t newDataSize = toBeReturned.N.dataSize;
	
	if (isReadOnly(arg2)) {
		if (!nodes[self].cache.N.data) {
			printf("mallocing data from node %i, in eval_limitedDynamicFillByte4D2\n", self);
			nodes[self].cache.N.data = malloc(newDataSize);
			addLoadedNode(self);
		}
		else if (nodes[self].cache.N.dataSize < newDataSize) {
			printf("reallocing data from node %i, in eval_limitedDynamicFillByte4D2\n", self);
			nodes[self].cache.N.data = 
				realloc(nodes[self].cache.N.data, newDataSize);
		}
		nodes[self].cache.N.dataSize = newDataSize;
		toBeReturned.N.data = nodes[self].cache.N.data;
		memcpy(toBeReturned.N.data, source.N.data, newDataSize);
	}
	
	double rectX = rect.n4[0];
	double rectY = rect.n4[1];
	double rectW = rect.n4[2];
	double rectH = rect.n4[3];
	
	//clip the rectangle to prevent out-of-bounds access
	if (
		rectW < 1 || 
		rectX > toBeReturned.N.dimenX ||
		rectY > toBeReturned.N.dimenY
	) {
		return toBeReturned;
	}
	if (rectX < 0)
		rectX = 0;
	if (rectY < 0)
		rectY = 0;
	if (rectH < 1)
		rectH = 1;
	if (rectX + rectW  >  toBeReturned.N.dimenX)
		rectW = toBeReturned.N.dimenX - rectX;
	if (rectY + rectH  >  toBeReturned.N.dimenY)
		rectH = toBeReturned.N.dimenY - rectY;
	
	//filler arguments:
	//	xPos
	//	yPos
	//	sourceWidth
	//	sourceHeight
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = rectW;
	fillerCallArgs[3].n = rectH;
	number *dataToBeReturned = toBeReturned.N.data;
	for (int yPos = rectY; yPos  <  rectY + rectH; yPos++) {
		fillerCallArgs[1].n = yPos - rectY;
		for (int xPos = rectX; xPos  <  rectX + rectW; xPos++) {
			fillerCallArgs[0].n = xPos - rectX;
			outType value = _output_(filler.f + 1, fillerCallArgs)
			int dataPos = (yPos * toBeReturned.N.dimenX) + xPos;
			dataToBeReturned[dataPos] = value.n;
		}
	}
	
	return toBeReturned;
}
const stdNode node_limitedDynamicFillNumD2 = {
	.name = "limitedDynamicFillNumD2 numD2\nfiller num & num num num num\nrect num4\nsource numD2",
	.arity = 3,
	.evaluate = eval_limitedDynamicFillNumD2
};


outType eval_randomBoolFillNumD2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	outType source = _output_(arg0, fnCallArgs);
	
	outType toBeReturned = source;
	size_t newDataSize = toBeReturned.N.dataSize;
	
	if (isReadOnly(arg0)) {
		if (!nodes[self].cache.N.data) {
			printf("mallocing data from node %i, in eval_randomBoolFillNumD2\n", self);
			nodes[self].cache.N.data = malloc(newDataSize);
			addLoadedNode(self);
		}
		else if (nodes[self].cache.N.dataSize < newDataSize) {
			printf("reallocing data from node %i, in eval_randomBoolFillNumD2\n", self);
			nodes[self].cache.N.data = 
				realloc(nodes[self].cache.N.data, newDataSize);
		}
		nodes[self].cache.N.dataSize = newDataSize;
		toBeReturned.N.data = nodes[self].cache.N.data;
	}
	
	number *dataToBeReturned = toBeReturned.N.data;
	int elements = toBeReturned.N.dimenX * toBeReturned.N.dimenY;
	for (int i = 0; i < elements; i++)
		dataToBeReturned[i] = rand()%2;
	
	return toBeReturned;
}
const stdNode node_randomBoolFillNumD2 = {
	.name = "randomBoolFillNumD2 numD2\nsource numD2",
	.arity = 1,
	.evaluate = eval_randomBoolFillNumD2
};



outType eval_fromNumD2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	outType x      = _output_(arg0, fnCallArgs);
	outType y      = _output_(arg1, fnCallArgs);
	outType source = _output_(arg2, fnCallArgs);
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
const stdNode node_fromNumD2 = {
	.name = "fromNumD2 num\nx num\ny num\nsource numD2",
	.arity = 3,
	.evaluate = eval_fromNumD2
};



outType eval_widthOf(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = _output_(arg, fnCallArgs)
	outType toBeReturned;
	toBeReturned.n = argOut.B.dimenX;
	return toBeReturned;
}
const stdNode node_widthOf = {
	.name = "widthOf num\narray array",
	.arity = 1,
	.evaluate = eval_widthOf
};

outType eval_heightOf(_evalargs_) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = _output_(arg, fnCallArgs)
	outType toBeReturned;
	toBeReturned.n = argOut.B.dimenY;
	return toBeReturned;
}
const stdNode node_heightOf = {
	.name = "heightOf num\narray array",
	.arity = 1,
	.evaluate = eval_heightOf
};


