
#define   maxLineLength    120
#define   maxTypeDefLength  40
#define   commentChar  ';'

FILE     *fileStream;
//char      fileChar;
char      lineBuf[maxLineLength];
uint8_t   expectedIndentation = 0;
uint32_t  currentLine = 0;

bool  reachedEOF = false;
bool  noErrors = true;


#define    nodePage     20
int        nodeSpace   = 0;
nodeIndex  currentNode = 0;

#define    rootNodePage     10
int        rootNodeSpace   = 0;
int        currentRootNode = 0;

#define    framePage      10
#define    framePage      10
int        frameSpace   =  0;
int        currentFrame = -1; //-1 until we get to the 0th frame


void  error_leadingSpace(void) {
	noErrors = false;
	printf("error: leading space at line %d\n", currentLine);
}
void  error_overIndented(void) {
	noErrors = false;
	printf("error: line %d is over-indented\n", currentLine);
}



void  getLine(void) {
	currentLine++;
	
	for (int lineCharIndex = 0;; lineCharIndex++) {
		
		//check to be sure we are not overflowing the buffer
		if (lineCharIndex == maxLineLength) {
			noErrors = false;
			printf(
				"error: token at line %d exceeds maxLineLength\n", 
				currentLine
			);
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
			lineBuf[lineCharIndex] == commentChar
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
			if (lineBuf[lineCharIndex] == commentChar) {
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
void  getSet(void) {}

void  getRoot(void) {}
void  getFrame(void) {
	currentFrame++;
	
	//if frameSpace is full then reallocate
	if (currentFrame == frameSpace) {
		frame  *prevFrameSpace = frameSpace;
		frameSpace += framePage;
		frameSpace = malloc( sizeof(frame)*frameSpace );
		for (int i = 0; i < currentFrame; i++)
			frameSpace[i] = prevFrameSpace[i];
		free(prevFrameSpace);
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
			noErrors = false;
			printf("error: invalid frame name at line %d\n", currentLine);
			return;
		}
		else break;
	}
	
	//allocate space for .name, initialize .outputCount
	frameSpace[currentFrame].name = malloc( sizeof(char)*maxLineLength );
	frameSpace[currentFrame].outputCount = 0;
	
	//copy from lineBuf to .name
	int namePos = 0;
	while (true) {
		frameSpace[currentFrame].name[namePos] = lineBuf[bufPos];
		if (lineBuf[bufPos] == '\0')
			break;
		namePos++;
		bufPos++;
	}
}
void  getOutput(void) {}

void  parse(void) {
	while (noErrors) {
		getLine();
		
		//if the line is blank or commented then skip it
		if (lineBuf[0] == '\0')
			continue;
		
		//if the line starts with a space or tab then error
		else if (lineBuf[0] == ' ') {
			error_leadingSpace();
			return;
		}
		else if (lineBuf[0] == '\t') {
			error_overIndented();
			return;
		}
		
		//check for new frame
		else if (lineBuf[0] = '#') {
			getFrame();
		}
		
		//we must have either a root or an output
		else {
			if (currentFrame < 0)
				getRoot();
			else
				getOutput();
		}
	}
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
						getFrame();
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


