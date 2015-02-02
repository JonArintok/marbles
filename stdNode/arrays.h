

outType eval_buildB4D2(_evalargs_) {
	nodeIndex arg0 = nodes[self].children[0];
	nodeIndex arg1 = nodes[self].children[1];
	outType widthSource  = output(arg0, -1, fnCallArgs);
	outType heightSource = output(arg1, -1, fnCallArgs);
	size_t newDataSize = sizeof(byte) * 4 * widthSource.n * heightSource.n;
	
	setLoadedNode(self, newDataSize);
	memset(nodes[self].cache.B.data, 0, newDataSize);
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
	outType widthSource  = output(arg0, -1, fnCallArgs);
	outType heightSource = output(arg1, -1, fnCallArgs);
	size_t newDataSize = sizeof(number) * widthSource.n * heightSource.n;
	
	setLoadedNode(self, newDataSize);
	memset(nodes[self].cache.N.data, 0, newDataSize);
	nodes[self].cache.N.dimenX = widthSource.n;
	nodes[self].cache.N.dimenY = heightSource.n;
	nodes[self].cache.N.dimenZ = 1;
	
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
	outType value  = output(arg0, -1, fnCallArgs);
	outType source = output(arg1, -1, fnCallArgs);
	
	outType toBeReturned = source;
	size_t newDataSize = toBeReturned.B.dataSize;
	
	if (isReadOnly(arg1)) {
		setLoadedNode(self, newDataSize);
		toBeReturned.B.data = nodes[self].cache.B.data;
	}
	
	byte *dataToBeReturned = toBeReturned.B.data;
	for (int i = 0; i < newDataSize;) {
		dataToBeReturned[i++] = value.bt[0];
		dataToBeReturned[i++] = value.bt[1];
		dataToBeReturned[i++] = value.bt[2];
		dataToBeReturned[i++] = value.bt[3];
	}
	
	return toBeReturned;
}
const stdNode node_fillB4D2 = {
	.name = "fillB4D2 B4D2\nvalue B4\nsource B4D2",
	.arity = 2,
	.evaluate = eval_fillB4D2
};



outType eval_mapB4D2(_evalargs_) {
	outType filler;
	outType toBeReturned;
	
	if (taskPiece < 0) {
		outType newFnCallArgs[maxChildren];
		nodeIndex arg0 = nodes[self].children[0];
		nodeIndex arg1 = nodes[self].children[1];
		newFnCallArgs[0] = output(arg0, -1, fnCallArgs); //filler
		outType source   = output(arg1, -1, fnCallArgs);
		
		toBeReturned = source;
		size_t newDataSize = toBeReturned.N.dataSize;
		
		if (isReadOnly(arg1)) {
			setLoadedNode(self, newDataSize);
			toBeReturned.N.data = nodes[self].cache.N.data;
		}
		
		newFnCallArgs[1] = toBeReturned;
		return initTask(self, newFnCallArgs);
	}
	else {
		filler       = fnCallArgs[0];
		toBeReturned = fnCallArgs[1];
	}
	
	int yPos = 0;
	int yLimit = toBeReturned.B.dimenY;
	
	if (toBeReturned.B.dimenY >= threadCount) {
		int perThread = toBeReturned.B.dimenY / threadCount;
		yPos = taskPiece * perThread;
		if (taskPiece == threadCount-1)
			yLimit = toBeReturned.B.dimenY;
		else
			yLimit = (taskPiece+1) * perThread;
	}
	else if (taskPiece) {
		return toBeReturned;
	}
	
	//filler arguments: x, y, width, height
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = toBeReturned.B.dimenX;
	fillerCallArgs[3].n = toBeReturned.B.dimenY;
	byte *dataToBeReturned = toBeReturned.B.data;
	int dataPos = yPos * toBeReturned.B.dimenX * 4;
	for (; yPos < yLimit; yPos++) {
		fillerCallArgs[1].n = yPos;
		for (int xPos = 0; xPos < toBeReturned.B.dimenX; xPos++) {
			fillerCallArgs[0].n = xPos;
			outType value = output(filler.f+1, -1, fillerCallArgs);
			dataToBeReturned[dataPos++] = value.bt[0];
			dataToBeReturned[dataPos++] = value.bt[1];
			dataToBeReturned[dataPos++] = value.bt[2];
			dataToBeReturned[dataPos++] = value.bt[3];
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
	outType filler;
	outType toBeReturned;
	
	if (taskPiece < 0) {
		outType newFnCallArgs[maxChildren];
		nodeIndex arg0 = nodes[self].children[0];
		nodeIndex arg1 = nodes[self].children[1];
		newFnCallArgs[0] = output(arg0, -1, fnCallArgs); //filler
		outType source   = output(arg1, -1, fnCallArgs);
		
		toBeReturned = source;
		size_t newDataSize = toBeReturned.N.dataSize;
		
		if (isReadOnly(arg1)) {
			setLoadedNode(self, newDataSize);
			toBeReturned.N.data = nodes[self].cache.N.data;
		}
		
		newFnCallArgs[1] = toBeReturned;
		return initTask(self, newFnCallArgs);
	}
	else {
		filler       = fnCallArgs[0];
		toBeReturned = fnCallArgs[1];
	}
	
	int yPos = 0;
	int yLimit = toBeReturned.N.dimenY;
	
	if (toBeReturned.N.dimenY >= threadCount) {
		int perThread = toBeReturned.N.dimenY / threadCount;
		yPos = taskPiece * perThread;
		if (taskPiece == threadCount-1)
			yLimit = toBeReturned.N.dimenY;
		else
			yLimit = (taskPiece+1) * perThread;
	}
	else if (taskPiece) {
		return toBeReturned;
	}
	
	//filler arguments: x, y, width, height
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = toBeReturned.N.dimenX;
	fillerCallArgs[3].n = toBeReturned.N.dimenY;
	number *dataToBeReturned = toBeReturned.N.data;
	int     dataPos          = yPos * toBeReturned.N.dimenX;
	for (; yPos < yLimit; yPos++) {
		fillerCallArgs[1].n = yPos;
		for (int xPos = 0; xPos < toBeReturned.N.dimenX; xPos++) {
			fillerCallArgs[0].n = xPos;
			outType value   = output(filler.f+1, -1, fillerCallArgs);
			dataToBeReturned[dataPos++] = value.n;
		}
	}
	
	return toBeReturned;
}
const stdNode node_mapN1D2 = {
	.name = "mapN1D2 N1D2\nfiller N1 & N1 N1 N1 N1\nsource N1D2",
	.arity = 2,
	.evaluate = eval_mapN1D2
};

outType eval_mapThruB4D2(_evalargs_) {
	if (taskPiece < 0) {
		outType newFnCallArgs[maxChildren];
		nodeIndex arg0 = nodes[self].children[0];
		nodeIndex arg1 = nodes[self].children[1];
		newFnCallArgs[0] = output(arg0, -1, fnCallArgs); //filler
		newFnCallArgs[1] = output(arg1, -1, fnCallArgs); //source
		
		size_t newDataSize = newFnCallArgs[1].B.dataSize;
		setLoadedNode(self, newDataSize);
		nodes[self].cache.B.dimenX = newFnCallArgs[1].B.dimenX;
		nodes[self].cache.B.dimenY = newFnCallArgs[1].B.dimenY;
		nodes[self].cache.B.dimenZ = newFnCallArgs[1].B.dimenZ;
		
		return initTask(self, newFnCallArgs);
	}
	
	outType filler       = fnCallArgs[0];
	outType source       = fnCallArgs[1];
	outType toBeReturned = nodes[self].cache;
	
	int yPos = 0;
	int yLimit = toBeReturned.B.dimenY;
	
	if (toBeReturned.B.dimenY >= threadCount) {
		int perThread = toBeReturned.B.dimenY / threadCount;
		yPos = taskPiece * perThread;
		if (taskPiece == threadCount-1)
			yLimit = toBeReturned.B.dimenY;
		else
			yLimit = (taskPiece+1) * perThread;
	}
	else if (taskPiece) {
		return toBeReturned;
	}
	
	//filler arguments: x, y, width, height
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = toBeReturned.B.dimenX;
	fillerCallArgs[3].n = toBeReturned.B.dimenY;
	fillerCallArgs[4]   = source;
	byte *dataToBeReturned = toBeReturned.B.data;
	int dataPos = yPos * toBeReturned.B.dimenX * 4;
	for (; yPos < yLimit; yPos++) {
		fillerCallArgs[1].n = yPos;
		for (int xPos = 0; xPos < toBeReturned.B.dimenX; xPos++) {
			fillerCallArgs[0].n = xPos;
			outType value = output(filler.f+1, -1, fillerCallArgs);
			dataToBeReturned[dataPos++] = value.bt[0];
			dataToBeReturned[dataPos++] = value.bt[1];
			dataToBeReturned[dataPos++] = value.bt[2];
			dataToBeReturned[dataPos++] = value.bt[3];
		}
	}
	
	return toBeReturned;
}
const stdNode node_mapThruB4D2 = {
	.name = "mapThruB4D2 B4D2\nfiller B4 & N1 N1 N1 N1 B4D2\nsource B4D2",
	.arity = 2,
	.evaluate = eval_mapThruB4D2
};


outType eval_mapThruN1D2(_evalargs_) {
	if (taskPiece < 0) {
		outType newFnCallArgs[maxChildren];
		nodeIndex arg0 = nodes[self].children[0];
		nodeIndex arg1 = nodes[self].children[1];
		newFnCallArgs[0] = output(arg0, -1, fnCallArgs); //filler
		newFnCallArgs[1] = output(arg1, -1, fnCallArgs); //source
		
		size_t newDataSize = newFnCallArgs[1].N.dataSize;
		setLoadedNode(self, newDataSize);
		nodes[self].cache.N.dimenX = newFnCallArgs[1].N.dimenX;
		nodes[self].cache.N.dimenY = newFnCallArgs[1].N.dimenY;
		nodes[self].cache.N.dimenZ = newFnCallArgs[1].N.dimenZ;
		
		return initTask(self, newFnCallArgs);
	}
	
	outType filler       = fnCallArgs[0];
	outType source       = fnCallArgs[1];
	outType toBeReturned = nodes[self].cache;
	
	int yPos = 0;
	int yLimit = toBeReturned.N.dimenY;
	
	if (toBeReturned.N.dimenY >= threadCount) {
		int perThread = toBeReturned.N.dimenY / threadCount;
		yPos = taskPiece * perThread;
		if (taskPiece == threadCount-1)
			yLimit = toBeReturned.N.dimenY;
		else
			yLimit = (taskPiece+1) * perThread;
	}
	else if (taskPiece) {
		return toBeReturned;
	}
	
	//filler arguments: x, y, w, w, s
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = toBeReturned.N.dimenX;
	fillerCallArgs[3].n = toBeReturned.N.dimenY;
	fillerCallArgs[4]   = source;
	number *dataToBeReturned = toBeReturned.N.data;
	int     dataPos          = yPos * toBeReturned.N.dimenX;
	for (; yPos < yLimit; yPos++) {
		fillerCallArgs[1].n = yPos;
		for (int xPos = 0; xPos < toBeReturned.N.dimenX; xPos++) {
			fillerCallArgs[0].n = xPos;
			outType value = output(filler.f+1, -1, fillerCallArgs);
			dataToBeReturned[dataPos++] = value.n;
		}
	}
	
	return toBeReturned;
}
const stdNode node_mapThruN1D2 = {
	.name = "mapThruN1D2 N1D2\nfiller N1 & N1 N1 N1 N1 N1D2\nsource N1D2",
	.arity = 2,
	.evaluate = eval_mapThruN1D2
};


bool outOfBounds(outType rect, int dimenX, int dimenY) {
	return 
		rect.nt[0] >= dimenX         ||
		rect.nt[1] >= dimenY         ||
		rect.nt[0] + rect.nt[2] <= 0 ||
		rect.nt[1] + rect.nt[3] <= 0 ||
		rect.nt[2] < 1               ||
		rect.nt[3] < 1
	;
}

outType eval_mapInB4D2(_evalargs_) {
	outType filler;
	outType rect;
	outType toBeReturned;
	
	if (taskPiece < 0) {
		outType newFnCallArgs[maxChildren];
		nodeIndex arg0 = nodes[self].children[0];
		nodeIndex arg1 = nodes[self].children[1];
		nodeIndex arg2 = nodes[self].children[2];
		newFnCallArgs[0] = output(arg0, -1, fnCallArgs);
		rect             = output(arg1, -1, fnCallArgs);
		outType source   = output(arg2, -1, fnCallArgs);
		
		toBeReturned = source;
		if (isReadOnly(arg2)) {
			setLoadedNode(self, toBeReturned.B.dataSize);
			toBeReturned.B.data = nodes[self].cache.B.data;
			memcpy(toBeReturned.B.data, source.B.data, toBeReturned.B.dataSize);
		}
		newFnCallArgs[2] = toBeReturned;
		
		if (outOfBounds(rect, toBeReturned.B.dimenX, toBeReturned.B.dimenY))
			return toBeReturned;
		newFnCallArgs[1] = rect;
		
		return initTask(self, newFnCallArgs);
	}
	else {
		filler       = fnCallArgs[0];
		rect         = fnCallArgs[1];
		toBeReturned = fnCallArgs[2];
	}
	
	const int rectX  = rect.nt[0];
	const int rectY  = rect.nt[1];
	const int rectW  = rect.nt[2];
	const int rectH  = rect.nt[3];
	const int dimenX = toBeReturned.B.dimenX;
	const int dimenY = toBeReturned.B.dimenY;
	
	int yPos   = rectY;
	int xStart = rectX < 0  ?  0  :  rectX;
	int yLimit = rectY + rectH;
	int xLimit = rectX + rectW;
	
	if (rectH <= taskPiece)
		return toBeReturned;
	else {
		int perThread = rectH / threadCount;
		yPos = rectY + taskPiece*perThread;
		if (taskPiece == threadCount-1)
			yLimit = rectY + rectH;
		else
			yLimit = rectY + (taskPiece+1)*perThread;
	}
	
	if (xLimit > dimenX)
		xLimit = dimenX;
	if (yPos < 0)
		yPos = 0;
	if (yLimit > dimenY) {
		if (yPos > dimenY)
			return toBeReturned;
		yLimit = dimenY;
	}
	
	//filler arguments: x, y, width, height
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = rectW;
	fillerCallArgs[3].n = rectH;
	
	byte *dataToBeReturned = toBeReturned.B.data;
	for (; yPos < yLimit; yPos++) {
		fillerCallArgs[1].n = yPos - rectY;
		int dataPos = (yPos*dimenX + xStart)*4;
		for (int xPos = xStart; xPos < xLimit; xPos++) {
			fillerCallArgs[0].n = xPos - rectX;
			outType value   = output(filler.f+1, -1, fillerCallArgs);
			dataToBeReturned[dataPos++] = value.bt[0];
			dataToBeReturned[dataPos++] = value.bt[1];
			dataToBeReturned[dataPos++] = value.bt[2];
			dataToBeReturned[dataPos++] = value.bt[3];
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
	outType filler;
	outType rect;
	outType toBeReturned;
	
	if (taskPiece < 0) {
		outType newFnCallArgs[maxChildren];
		nodeIndex arg0 = nodes[self].children[0];
		nodeIndex arg1 = nodes[self].children[1];
		nodeIndex arg2 = nodes[self].children[2];
		newFnCallArgs[0] = output(arg0, -1, fnCallArgs);
		rect             = output(arg1, -1, fnCallArgs);
		outType source   = output(arg2, -1, fnCallArgs);
		
		toBeReturned = source;
		if (isReadOnly(arg2)) {
			setLoadedNode(self, toBeReturned.N.dataSize);
			toBeReturned.N.data = nodes[self].cache.N.data;
			memcpy(toBeReturned.N.data, source.N.data, toBeReturned.N.dataSize);
		}
		newFnCallArgs[2] = toBeReturned;
		
		if (outOfBounds(rect, toBeReturned.N.dimenX, toBeReturned.N.dimenY))
			return toBeReturned;
		newFnCallArgs[1] = rect;
		
		return initTask(self, newFnCallArgs);
	}
	else {
		filler       = fnCallArgs[0];
		rect         = fnCallArgs[1];
		toBeReturned = fnCallArgs[2];
	}
	
	const int rectX  = rect.nt[0];
	const int rectY  = rect.nt[1];
	const int rectW  = rect.nt[2];
	const int rectH  = rect.nt[3];
	const int dimenX = toBeReturned.N.dimenX;
	const int dimenY = toBeReturned.N.dimenY;
	
	int yPos   = rectY;
	int xStart = rectX < 0  ?  0  :  rectX;
	int yLimit = rectY + rectH;
	int xLimit = rectX + rectW;
	
	if (rectH <= taskPiece)
		return toBeReturned;
	else {
		int perThread = rectH / threadCount;
		yPos = rectY + taskPiece*perThread;
		if (taskPiece == threadCount-1)
			yLimit = rectY + rectH;
		else
			yLimit = rectY + (taskPiece+1)*perThread;
	}
	
	if (xLimit > dimenX)
		xLimit = dimenX;
	if (yPos < 0)
		yPos = 0;
	if (yLimit > dimenY) {
		if (yPos > dimenY)
			return toBeReturned;
		yLimit = dimenY;
	}
	
	//filler arguments: x, y, width, height
	outType fillerCallArgs[maxChildren];
	fillerCallArgs[2].n = rectW;
	fillerCallArgs[3].n = rectH;
	
	number *dataToBeReturned = toBeReturned.N.data;
	for (; yPos < yLimit; yPos++) {
		fillerCallArgs[1].n = yPos - rectY;
		int dataPos = yPos*dimenX + xStart;
		for (int xPos = xStart; xPos < xLimit; xPos++) {
			fillerCallArgs[0].n = xPos - rectX;
			outType value = output(filler.f+1, -1, fillerCallArgs);
			dataToBeReturned[dataPos++] = value.n;
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
	outType source = output(arg0, -1, fnCallArgs);
	
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
	outType x      = output(arg0, -1, fnCallArgs);
	outType y      = output(arg1, -1, fnCallArgs);
	outType source = output(arg2, -1, fnCallArgs);
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
	outType argOut = output(arg, -1, fnCallArgs);
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
	outType argOut = output(arg, -1, fnCallArgs);
	outType toBeReturned;
	toBeReturned.n = argOut.B.dimenY;
	return toBeReturned;
}
const stdNode node_heightOf = {
	.name = "heightOf N1\narray N3",
	.arity = 1,
	.evaluate = eval_heightOf
};


