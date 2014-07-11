

#define   commentInitializer  ';'

FILE     *fileStream;
char     *fileName
char      lineBuf[maxLineLength];
uint8_t   expectedIndentation = 0;
uint32_t  currentLine = 0;

bool  reachedEOF = false;
bool  noErrors = true;

bool  inFrameform = false;


#define    nodePage      20
#define    nodeNamePage  20
int        nodeSpace    = 0;
nodeIndex  currentNode  = 0;

#define    rootNodePage     10
int        rootNodeSpace   = 0;
int        currentRootNode = 0;

#define    frameformPage      10
#define    stateNodePage      10
int        frameformSpace   =  0;
int        currentFrameform =  0;



void  putError(char *message) {
	noErrors = false;
	printf(
		"error in line %d of %s :\n\t%s\n", 
		currentLine, fileName, message
	);
}



void  getLine(void) {
	currentLine++;
	
	for (int lineCharIndex = 0;; lineCharIndex++) {
		
		//check to be sure we are not overflowing the buffer
		if (lineCharIndex == maxLineLength) {
			putError("line is too long");
			return;
		}
		
		//get char from file
		lineBuf[lineCharIndex] = fgetc(fileStream);
		
		//check for EOF
		if (lineBuf[lineCharIndex] == EOF) {
			reachedEOF = true;
			return;
		}
		
		//check for linebreaks, comments, and eliminate trailing whitespace
		if (
			lineBuf[lineCharIndex] == '\n' ||
			lineBuf[lineCharIndex] == commentInitializer
		) {
			//null terminate the buffer
			lineBuf[lineCharIndex] = '\0';
			
			//remove trailing whitespace, if any
			int backstep = 1;
			while (lineBuf[lineCharIndex-backstep] == ' ') {
				lineBuf[lineCharIndex-backstep] = '\0';
				backstep++;
			}
			
			//if we hit a comment, read through the rest of the comment
			if (lineBuf[lineCharIndex] == commentInitializer) {
				char c = fgetc(fileStream);
				if (c == '\n')
					return;
				if (c == EOF) {
					reachedEOF = true;
					return;
				}
			}
		}
	}
}

void  getBranch(void) {}

void  getFn(void) {}
void  getVar(void) {}

void  getRootNode(void) {}
void  getFrameform(void) {
	inFrameform = true;
	
	//if frameforms is full then reallocate
	if (currentFrameform == frameformSpace) {
		frameformSpace += frameformPage;
		if (currentFrameform == 0)
			frameforms = malloc( sizeof(frameform)*frameformSpace )
		else 
			frameforms = realloc(
				frameforms, 
				sizeof(frameform)*frameformSpace
			);
	}
	
	//ignore spaces between the '#' and the name
	int bufPos = 1;
	while (true) {
		if (lineBuf[bufPos] == ' ')
			bufPos++;
		else if (
			lineBuf[bufPos] == '\0' || 
			lineBuf[bufPos] >=  '0' || 
			lineBuf[bufPos] <=  '9'
		) {
			putError("invalid frameform name");
			return;
		}
		else break;
	}
	
	//allocate space for .name, initialize .currentStateNode
	frameforms[currentFrameform].name
		= malloc( sizeof(char)*maxTokenLength );
	frameforms[currentFrameform].currentStateNode = 0;
	frameforms[currentFrameform].stateNodeSpace = 0;
	
	//copy from lineBuf to frameforms[currentFrameform].name
	int namePos = 0;
	while (true) {
		frameforms[currentFrameform].name[namePos] = lineBuf[bufPos];
		if (lineBuf[bufPos] == '\0')
			break;
		namePos++;
		bufPos++;
	}
	currentFrameform++;
}

void  getStateNode(void) {
	
	//if nodes is full then allocate/reallocate
	if (currentNode == nodeSpace) {
		nodeSpace += nodePage;
		if (currentNode == 0)
			nodes = malloc( sizeof(node)*nodeSpace );
		else
			nodes = realloc( nodes, sizeof(node)*nodeSpace );
	}
	
	//if currentFrameform.stateNodes is full then allocate/reallocate
	if (
		currentFrameform.currentStateNode == 
		currentFrameform.stateNodeSpace
	) {
		currentFrameform.stateNodeSpace += stateNodePage;
		if (currentFrameform.currentStateNode == 0)
			currentFrameform.stateNodes
				= malloc( sizeof(nodeIndex)*currentFrameform.stateNodeSpace );
		else
			currentFrameform.stateNodes 
				= realloc(
					currentFrameform.stateNodes,
					sizeof(nodeIndex)*currentFrameform.stateNodeSpace
				)
			;
	}
	
	//the current stateNode is the current node
	currentFrameform.currentStateNode = currentNode;
	
	
	//copy the name from lineBuf into the stateNode
	char  *stateNodeNameDest
		= currentFrameform.stateNodes[
			currentFrameform.currentStateNode
		].name
	;
	stateNodeNameDest = malloc( sizeof(char)*nodeNamePage );
	int nodeNameSpace = nodeNamePage;
	int namePos = 0;
	int bufPos  = 1;
	while (lineBuf[bufPos] == ' ')
		bufPos++;
	while (true) {
		if (namePos)
		stateNodeNameDest[namePos] = lineBuf[bufPos];
		if (lineBuf[bufPos] == '\0')
			break;
		namePos++;
		bufPos++;
	}
	
	//the next node must be the branch of the stateNode
	currentNode++;
	getBranch();
	
	currentFrameform.currentStateNode++;
}

void  parse(void) {
	//go through the file(s) line by line
	while (noErrors) {
		getLine();
		
		//if the line is blank or commented then skip it
		if (lineBuf[0] == '\0')
			continue;
		
		//if the line starts with a space or tab then error
		else if (lineBuf[0] == ' ') {
			putError("leading space");
			return;
		}
		else if (lineBuf[0] == '\t') {
			putError("over-idented");
			return;
		}
		
		//check for beginning or end of frameform
		else if (lineBuf[0] = '#') {
			if (lineBuf[1] = '#')
				inFrameform = false;
			else
				getFrameform();
		}
		
		//we must have either a root or a stateNode
		else {
			if (inFrameform)
				getStateNode();
			else
				getRootNode();
		}
	}
	
	//check for type errors
	
	
}















int  lookupNode(char *nameIn) {
	
	//then match the name to a node from the nodeDefTable
	for (
		int nodeDefTableIndex = 0;
		nodeDefTableIndex < nodeDefTableLength; 
		nodeDefTableIndex++
	) {
		//get typeDefLength from the node's name
		int typeDefLength = 0;
		//typeDefLength should be at least 1
		while (true) {
			typeDefLength++;
			if (
				nodeDefTable[nodeDefTableIndex]->name[typeDefLength] == ' '
				&& nodeDefTable[nodeDefTableIndex]->name[typeDefLength+1] == ' '
				&& nodeDefTable[nodeDefTableIndex]->name[typeDefLength+2] != ' '
			) {
				typeDefLength += 2;
				break;
			}
		}
		//match the names
		for (
			int tokenCharIndex = 0;
			tokenCharIndex < maxTokenLength; 
			tokenCharIndex++
		) {
			if (
				nodeDefTable[nodeDefTableIndex]
				-> name[tokenCharIndex+typeDefLength]
				== nameIn[tokenCharIndex]
			) 
				continue;
			else if (
				nodeDefTable[nodeDefTableIndex]
				-> name[tokenCharIndex+typeDefLength] == '\n'
				&& nameIn[tokenCharIndex] == '\0'
			)
				return nodeDefTableIndex;
			else
				break;//go to the next nodeDefTableIndex
		}
	}
	//if you're here, then the name wasn't recognized...
	return -1;
}

void  getNode() {
	
	for (int tokenCharIndex = 0;; tokenCharIndex++) {
		if (tokenCharIndex == maxTokenLength) {
			noErrors = false;
			printf(
				"error: token at line %d exceeds maxTokenLength\n", 
				currentLine
			);
			return;
		}
		
		fileChar = fgetc(fileStream);
		
		//check the first character
		if (!tokenCharIndex) {
			switch (fileChar) {
				case ' ':
					noErrors = false;
					printf("error: leading space at line %d\n", currentLine);
					return;
				case '\t':
					noErrors = false;
					printf("error: line %d is over-indented\n", currentLine);
					return;
				case '#':
					if (expectedIndentation == 0) {
						getFrameform();
						return;
					}
					else tokenBuf[tokenCharIndex] = fileChar;
					break;
			}
			continue;
		}
		
		if (fileChar == EOF) {
			reachedEOF = true;
			if (expectedIndentation > 0) {
				noErrors = false;
				printf(
					"error: file ended unexpectadly at line %d\n", 
					currentLine
				);
			}
			return;
		}
		else if (fileChar == '\n'  ||  fileChar == commentChar) {
			//null terminate the token
			tokenBuf[tokenCharIndex] = '\0';
			
			//remove trailing whitespace, if any
			int backstep = 1;
			while (tokenBuf[tokenCharIndex-backstep] == ' ') {
				tokenBuf[tokenCharIndex-backstep] = '\0';
				backstep++;
			}
			
			//if we hit a comment, read through the rest of the comment
			if (fileChar == commentChar) {
				for (;; tokenCharIndex++) {
					fileChar = fgetc(fileStream);
					if (fileChar == '\n')
						break;
					if (fileChar == EOF) {
						reachedEOF = true;
						if (expectedIndentation > 0) {
							noErrors = false;
							printf(
								"error: file ended unexpectadly at line %d\n", 
								currentLine
							);
						}
						return;
					}
				}
			}
			currentLine++;
			break;
		}
		else tokenBuf[tokenCharIndex] = fileChar;
	}
	
	//if the line was blank or commented...
	if (tokenBuf[0] == '\0') {
		//if it was not at the root level
		if (expectedIndentation > 0) {
			noErrors = false;
			printf("error: expected an argument at line %d\n", currentLine-1);
		}
		return;
	}
	
	//check for number literal
	if (tokenBuf[0] >= '0'  &&  tokenBuf[0] <= '9') {
		//make sure it's a valid number
		for (
			int tokenCharIndex = 0; 
			;//tokenCharIndex < maxTokenLength; 
			tokenCharIndex++
		) {
			if (tokenBuf[tokenCharIndex] == '\0')
				break;
			if (
				(
					tokenBuf[tokenCharIndex] < '0' || 
					tokenBuf[tokenCharIndex] > '9'
				) && tokenBuf[tokenCharIndex] != '.'
			) {
				noErrors = false;
				printf(
					"error: invalid number literal '%s' at line %d\n",
					tokenBuf, currentLine-1
				);
				return;
			}
		}
		
		nodes[currentNode] = node_numLit;
		sscanf(tokenBuf, "%lf", &nodes[currentNode].output.n);
		currentNode++;
		// reallocate the nodes to a larger array if necessary
		return;
	}
	
	
	int nodeToGet = lookupNode(tokenBuf);
	if (nodeToGet == -1) {
		noErrors = false;
		printf(
			"error: '%s' at line %d is not recognized\n", 
			tokenBuf, currentLine
		);
		return;
	}
	
	nodes[currentNode] = *nodeDefTable[nodeToGet];
	
	
	// get arguments, if any
	if (nodes[currentNode].arity) {
		nodeIndex parent = currentNode;
		currentNode++;
		expectedIndentation++;
		int indentation = 0;
		
		for (
			nodeIndex currentArg = 0;
			currentArg < nodes[parent].arity && noErrors;
			currentArg++
		) {
			
			indentation = 0;
			
			while (indentation < expectedIndentation) {
				fileChar = fgetc(fileStream);
				if (fileChar == '\t') 
					indentation++;
				else if (fileChar == EOF) {
					noErrors = false;
					reachedEOF = true;
					printf(
						"error: file ended unexpectadly at line %d\n", 
						currentLine
					);
					return;
				}
				else {
					noErrors = false;
					printf("error: line %d is under-indented\n", currentLine);
					return;
				}
			}
			
			nodes[parent].arguments[currentArg] = currentNode;
			getNode();
			//check that the node is of the requiredType
		}
		
		expectedIndentation--;
	}
	else currentNode++;
	
	// reallocate the nodes to a larger array if necessary
}


