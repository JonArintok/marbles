

outType eval_buildByte4array2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType widthSource  = _output_(arg0, fnCallArgs)
	outType heightSource = _output_(arg1, fnCallArgs)
	int newDataSpace = widthSource.n * heightSource.n * 4;
	
	if (!nodes[self].cache.B.data) {
		nodes[self].cache.B.data = malloc(sizeof(byte) * newDataSpace);
		addLoadedNode(self);
	}
	else if (nodes[self].cache.B.dataSpace < newDataSpace) {
		nodes[self].cache.B.data = 
			realloc(nodes[self].cache.B.data, newDataSpace);
	}
	nodes[self].cache.B.dataSpace = newDataSpace;
	nodes[self].cache.B.dimenX = widthSource.n;
	nodes[self].cache.B.dimenY = heightSource.n;
	
	return nodes[self].cache;
}
const stdNode node_buildByte4array2 = {
	.name = "buildByte4'' byte4''\nwidth num\nheight num",
	.arity = 2,
	.evaluate = eval_buildByte4array2
};


outType eval_fillByte4array2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType value  = _output_(arg0, fnCallArgs);
	outType source = _output_(arg1, fnCallArgs);
	
	outType toBeReturned = source;
	int newDataSpace = toBeReturned.B.dataSpace;
	
	if (isReadOnly(source)) {
		if (!nodes[self].cache.B.data) {
			nodes[self].cache.B.data = malloc(sizeof(byte) * newDataSpace);
			addLoadedNode(self);
		}
		else if (nodes[self].cache.B.dataSpace < newDataSpace) {
			nodes[self].cache.B.data = 
				realloc(nodes[self].cache.B.data, newDataSpace);
		}
		nodes[self].cache.B.dataSpace = newDataSpace;
		toBeReturned.B.data = nodes[self].cache.B.data;
		setToWriteable(&toBeReturned);
	}
	
	byte *dataToBeReturned = toBeReturned.B.data;
	for (int i = 0; i < newDataSpace; i += 4) {
		dataToBeReturned[i  ] = value.b4[0];
		dataToBeReturned[i+1] = value.b4[1];
		dataToBeReturned[i+2] = value.b4[2];
		dataToBeReturned[i+3] = value.b4[3];
	}
	
	return toBeReturned;
}
const stdNode node_fillByte4array2 = {
	.name = "fillByte4'' byte4''\nvalue byte4\nsource byte4''",
	.arity = 2,
	.evaluate = eval_fillByte4array2
};



outType eval_dynamicFillByte4array2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType filler = _output_(arg0, fnCallArgs);
	outType source = _output_(arg1, fnCallArgs);
	
	outType toBeReturned = source;
	int newDataSpace = toBeReturned.B.dataSpace;
	
	if (isReadOnly(source)) {
		if (!nodes[self].cache.B.data) {
			nodes[self].cache.B.data = malloc(sizeof(byte) * newDataSpace);
			addLoadedNode(self);
		}
		else if (nodes[self].cache.B.dataSpace < newDataSpace) {
			nodes[self].cache.B.data = 
				realloc(nodes[self].cache.B.data, newDataSpace);
		}
		nodes[self].cache.B.dataSpace = newDataSpace;
		toBeReturned.B.data = nodes[self].cache.B.data;
		setToWriteable(&toBeReturned);
	}
	
	//filler arguments:
	//	x position
	//	y position
	//	source width
	//	source height
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
const stdNode node_dynamicFillByte4array2 = {
	.name = "dynamicFillByte4'' byte4''\nfiller byte4 & num num num num\nsource byte4''",
	.arity = 2,
	.evaluate = eval_dynamicFillByte4array2
};



outType eval_limitedDynamicFillByte4array2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	nodeIndex arg2 = nodes[self].children[2];
	outType filler = _output_(arg0, fnCallArgs);
	outType rect   = _output_(arg1, fnCallArgs);
	outType source = _output_(arg2, fnCallArgs);
	
	outType toBeReturned = source;
	int newDataSpace = toBeReturned.B.dataSpace;
	
	if (isReadOnly(source)) {
		if (!nodes[self].cache.B.data) {
			nodes[self].cache.B.data = malloc(sizeof(byte) * newDataSpace);
			addLoadedNode(self);
		}
		else if (nodes[self].cache.B.dataSpace < newDataSpace) {
			nodes[self].cache.B.data = 
				realloc(nodes[self].cache.B.data, newDataSpace);
		}
		nodes[self].cache.B.dataSpace = newDataSpace;
		toBeReturned.B.data = nodes[self].cache.B.data;
		setToWriteable(&toBeReturned);
		//copy source to toBeReturned
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
	
	//filler arguments:
	//	xPos
	//	yPos
	//	sourceWidth
	//	sourceHeight
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
const stdNode node_limitedDynamicFillByte4array2 = {
	.name = "limitedDynamicFillByte4'' byte4''\nfiller byte4 & num num num num\nrect num4\nsource byte4''",
	.arity = 3,
	.evaluate = eval_limitedDynamicFillByte4array2
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


