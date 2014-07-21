

#define   commentInitializer   ';'
#define   typeBodyBoundary     '='
#define   frameFormInitializer '#'

FILE     *fileStream;
char     *fileName;
char      lineBuf[maxLineLength];
uint8_t   expectedIndentation = 0;
uint32_t  currentLine = 0;

bool  reachedEOF = false;
bool  noErrors = true;

bool  inFrameform = false;


#define    nodePage       20
#define    nodeNamePage   20
int        nodeSpace    =  0;
nodeIndex  currentNode  = -1;

#define    rootNodePage      10
int        rootNodeSpace   =  0;
int        currentRootNode = -1;

#define    frameformPage       10
#define    stateNodePage       10
int        frameformSpace   =   0;
int        currentFrameform =  -1;



void  putError(char *message) {
	noErrors = false;
	printf(
		"error in line %d of %s : %s", 
		currentLine, fileName, message
	);
}



void  getLine(void) {
	currentLine++;
	
	for (int lineCharIndex = 0;; lineCharIndex++) {
		
		//check to be sure we are not overflowing the buffer
		if (lineCharIndex == maxLineLength) {
			putError("line is too long\n");
			return;
		}
		
		//get char from file
		lineBuf[lineCharIndex] = fgetc(fileStream);
		
		//check for EOF
		if (lineBuf[lineCharIndex] == EOF) {
			reachedEOF = true;
			return;
		}
		
		//newlines, comments, typeBodyBoundary, and trailing whitespace
		if (
			lineBuf[lineCharIndex] == '\n' ||
			lineBuf[lineCharIndex] == commentInitializer || 
			lineBuf[lineCharIndex] == typeBodyBoundary
		) {
			
			//if we hit the typeBodyBoundary, put the '\0' after, not over
			if (lineBuf[lineCharIndex] == typeBodyBoundary) {
				lineCharIndex++;
			}
			
			//if we're not at the end of the line, read through to the end
			if (lineBuf[lineCharIndex] != '\n') {
				char c;
				while ( ( c = fgetc(fileStream) ) != '\n' ) {
					if (c == EOF) {
						reachedEOF = true;
						return;
					}
				}
			}
			
			//null terminate the string
			lineBuf[lineCharIndex] = '\0';
			
			//remove trailing whitespace, if any
			if (lineCharIndex > 1) {
				int backstep = 1;
				while (lineBuf[lineCharIndex-backstep] == ' ') {
					lineBuf[lineCharIndex-backstep] = '\0';
					backstep++;
				}
			}
			
			//end of the line
			break;
		}
	}
	
	//check for leading space
	if (lineBuf[0] == ' ') {
		putError("leading space\n");
		return;
	}
	
	//remove indentation when reading bodies
	if (expectedIndentation) {
		
		//check for proper indentation
		int i = 0;
		for (; i < expectedIndentation; i++) {
			if (lineBuf[i] != '\t') {
				putError("under-indented\n");
				return;
			}
		}
		if (lineBuf[i+1] == '\t') {
			putError("over-indented\n");
			return;
		}
		
		//remove indentation from lineBuf
		for (
			int lineBufIndex = 0;
			lineBuf[lineBufIndex] != '\0';
			lineBufIndex++
		) {
			lineBuf[lineBufIndex] = lineBuf[expectedIndentation+lineBufIndex];
		}
	}
	
}

void  getArgs(void);
void  getNode(void) {
	currentNode++;
	
	//if nodes is full then allocate/reallocate
	if (currentNode == nodeSpace) {
		nodeSpace += nodePage;
		if (currentNode == 0)
			nodes = malloc( sizeof(node)*nodeSpace );
		else
			nodes = realloc( nodes, sizeof(node)*nodeSpace );
	}
	
	
	//check for rootNode or stateNode
	if (expectedIndentation == 0) {
		
		//either way
		nodes[currentNode].arity = 1;
		nodes[currentNode].arguments[0] = currentNode + 1;
		
		//get the .name of the node, which includes the type information
		nodes[currentNode].name = malloc( sizeof(char)*nodeNamePage );
		int nodeNameSpace = nodeNamePage;
		int namePos = 0;
		int lineBufPos = 0;
		while (true) {
			
			//reallocate if nodeNameSpace is full
			if (namePos < nodeNameSpace) {
				nodeNameSpace += nodeNamePage;
				nodes[currentNode].name = realloc(
					nodes[currentNode].name, 
					sizeof(char)*nodeNameSpace
				);
			}
			
			//check for newlines
			if (lineBuf[lineBufPos] == '\0') {
				nodes[currentNode].name[namePos] = '\n';
				getLine();
				lineBufPos = -1;
				if (lineBuf[0] == '\t') {
					if (lineBuf[1] == typeBodyBoundary) {
						//replace that \n with \0 and we're done
						nodes[currentNode].name[namePos] = '\0';
						break;
					}
				}
				else {
					putError("incomplete type declaration\n");
					return;
				}
			}
			//else copy the char
			else
				nodes[currentNode].name[namePos] = lineBuf[lineBufPos];
			
			namePos++;
			lineBufPos++;
		}
		
		//stateNode
		if (inFrameform) {
			//if currentFrameform.stateNodes is full then allocate/reallocate
			if (
				frameforms[currentFrameform].currentStateNode == 
				frameforms[currentFrameform].stateNodeSpace
			) {
				frameforms[currentFrameform].stateNodeSpace += stateNodePage;
				if (frameforms[currentFrameform].currentStateNode == 0) {
					frameforms[currentFrameform].stateNodes = malloc(
						sizeof(nodeIndex)*frameforms[
							currentFrameform
						].stateNodeSpace
					);
				}
				else {
					frameforms[currentFrameform].stateNodes = realloc(
						frameforms[currentFrameform].stateNodes,
						sizeof(nodeIndex)*frameforms[
							currentFrameform
						].stateNodeSpace
					);
				}
			}
			
			//the current stateNode is the current node
			nodes[currentNode].evaluate = eval_state;
			frameforms[currentFrameform].stateNodes[
				frameforms[currentFrameform].currentStateNode
			] = currentNode;
			frameforms[currentFrameform].currentStateNode++;
		}
		
		//rootNode
		else {
			
		}
		
		//the next node must be the body of this defNode
		getArgs();
		return;
	}
	
	//check for number literal
	if (lineBuf[0] >= '0' && lineBuf[0] <= '9') {
		//make sure it's a valid number
		for (int lineBufPos = 0;; lineBufPos++) {
			if (lineBuf[lineBufPos] == '\0')
				break;
			if (
				(
					lineBuf[lineBufPos] < '0' || 
					lineBuf[lineBufPos] > '9'
				) && 
				lineBuf[lineBufPos] != '.'
			) {
				putError("invalid number literal\n");
				return;
			}
		}
		
		//fill in the node, sscanf into the output
		nodes[currentNode] = node_numLit;
		sscanf(
			&lineBuf[0], 
			"%lf", 
			&nodes[currentNode].output.n
		);
		return;
	}
	
	//check for reference to stdNode
	for (int i = 0; i < stdNodeTableLength; i++) {
		int charIndex = 0;
		for (
			;
			lineBuf[charIndex] == stdNodeTable[i]->name[charIndex];
			charIndex++
		) {}
		if (
			lineBuf[charIndex] == '\0' &&
			stdNodeTable[i]->name[charIndex] == '\n'
		) {
			//we have a match
			nodes[currentNode] = *stdNodeTable[i];
			return;
		}
	}
	
	//check for reference to a rootNode
	
	
	//none of the above
	putError("not recognized : "); printf("%s\n", lineBuf);
}
void  getArgs(void) {
	expectedIndentation++;
	
	nodeIndex  parent = currentNode;
	for (
		int argIndex = 0;
		argIndex < nodes[parent].arity && noErrors;
		argIndex++
	) {
		getLine();
		getNode();
		nodes[parent].arguments[argIndex] = currentNode;
		if (nodes[currentNode].arity) {
			getArgs();
		}
	}
	
	expectedIndentation--;
}


void  getFrameform(void) {
	currentFrameform++;
	inFrameform = true;
	
	//if frameforms is full then reallocate
	if (currentFrameform == frameformSpace) {
		frameformSpace += frameformPage;
		if (currentFrameform == 0)
			frameforms = malloc( sizeof(frameform)*frameformSpace );
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
			(lineBuf[bufPos] >=  '0'  &&  lineBuf[bufPos] <=  '9')
		) {
			putError("invalid frameform name\n");
			return;
		}
		else break;
	}
	
	//copy from lineBuf to frameforms[currentFrameform].name
	int namePos = 0;
	while (true) {
		frameforms[currentFrameform].name[namePos] = lineBuf[bufPos];
		if (lineBuf[bufPos] == '\0')
			break;
		namePos++;
		bufPos++;
	}
	
	//initialize .currentStateNode and .stateNodeSpace and we're done
	frameforms[currentFrameform].currentStateNode = 0;
	frameforms[currentFrameform].stateNodeSpace = 0;
}


void  parse(void) {
	//go through the file(s) line by line
	while ( noErrors ) {
		getLine();
		
		//if we reached the end of the file then we're done
		if (reachedEOF)
			break;
		
		//if the line is blank or commented then skip it
		if (lineBuf[0] == '\0')
			continue;
		
		//check for beginning or end of frameform
		else if (lineBuf[0] == frameFormInitializer) {
			if (lineBuf[1] == frameFormInitializer)
				inFrameform = false;
			else
				getFrameform();
		}
		
		//we must have a node
		else
			getNode();
	}
	
	//check for type errors
	
}





