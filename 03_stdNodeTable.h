

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
	.name = "? any\ncondition num\nifTrue any\nelse any",
	.arity = 3,
	.evaluate = eval_if
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
	.evaluate = eval_not
};

outType eval_currentFrame(nodeIndex self, outType fnCallArgs[maxChildren]) {
	outType toBeReturned;
	toBeReturned.n = curFrame;
	return toBeReturned;
}
const stdNode node_curFrame = {
	.name = "currentFrame num",
	.arity = 0,
	.evaluate = eval_currentFrame
};





outType eval_windowWidth(nodeIndex self, outType fnCallArgs[maxChildren]) {
	outType toBeReturned;
	toBeReturned.n = windowWidth;
	return toBeReturned;
}
const stdNode node_windowWidth = {
	.name = "windowWidth num",
	.arity = 0,
	.evaluate = eval_windowWidth
};

outType eval_windowHeight(nodeIndex self, outType fnCallArgs[maxChildren]) {
	outType toBeReturned;
	toBeReturned.n = windowHeight;
	return toBeReturned;
}
const stdNode node_windowHeight = {
	.name = "windowHeight num",
	.arity = 0,
	.evaluate = eval_windowHeight
};






outType eval_buildByte4array2(
	nodeIndex self, outType fnCallArgs[maxChildren]
) {
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


outType eval_fillByte4array2(
	nodeIndex self, outType fnCallArgs[maxChildren]
) {
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




outType eval_fillByte4(nodeIndex self, outType fnCallArgs[maxChildren]) {
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




outType eval_byteFromNorm(nodeIndex self, outType fnCallArgs[maxChildren]) {
	nodeIndex arg = nodes[self].children[0];
	outType argOut = _output_(arg, fnCallArgs)
	outType toBeReturned;
	toBeReturned.b = argOut.n * 255;
	return toBeReturned;
}
const stdNode node_byteFromNorm = {
	.name = "byteFromNorm byte\ninput num",
	.arity = 1,
	.evaluate = eval_byteFromNorm
};








outType eval_byte4FromNorms(nodeIndex self, outType fnCallArgs[maxChildren]) {
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
const stdNode node_byte4FromNorms = {
	.name = "byte4FromNorms byte4\nb0 num\nb1 num\nb2 num\nb3 num",
	.arity = 4,
	.evaluate = eval_byte4FromNorms
};




outType eval_num4(nodeIndex self, outType fnCallArgs[maxChildren]) {
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







outType eval_widthOf(nodeIndex self, outType fnCallArgs[maxChildren]) {
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

outType eval_heightOf(nodeIndex self, outType fnCallArgs[maxChildren]) {
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



outType eval_dynamicFillByte4array2(
	nodeIndex self, outType fnCallArgs[maxChildren]
) {
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



outType eval_limitedDynamicFillByte4array2(
	nodeIndex self, outType fnCallArgs[maxChildren]
) {
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




outType eval_sqr(nodeIndex self, outType fnCallArgs[maxChildren]) {
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









outType eval_sin(nodeIndex self, outType fnCallArgs[maxChildren]) {
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


outType eval_frameRate(nodeIndex self, outType fnCallArgs[maxChildren]) {
	outType toBeReturned;
	toBeReturned.n = frameRate;
	return toBeReturned;
}
const stdNode node_frameRate = {
	.name = "frameRate num",
	.arity = 0,
	.evaluate = eval_frameRate
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
	&node_curFrame,
	&node_windowWidth,
	&node_windowHeight,
	&node_buildByte4array2,
	&node_fillByte4array2,
	&node_fillByte4,
	&node_byteFromNorm,
	&node_byte4FromNorms,
	&node_num4,
	&node_widthOf,
	&node_heightOf,
	&node_dynamicFillByte4array2,
	&node_limitedDynamicFillByte4array2,
	&node_sqr,
	&node_sin,
	&node_frameRate
};

