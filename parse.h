

#define   commentInitializer   '|'
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



void  putError(char *message) {
	noErrors = false;
	printf(
		"error in line %d of %s: %s", 
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
		
		//check for newlines and comments, and remove trailing whitespace
		if (
			lineBuf[lineCharIndex] == '\n' ||
			lineBuf[lineCharIndex] == commentInitializer
		) {
			
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
void  getDefNode(void) {
	//could be rootNode or stateNode, but either way...
	inc_currentNode();
	nodes[currentNode].arity = 1;
	nodes[currentNode].arguments[0] = currentNode + 1;
	
	//check for variable
	if ( strchr(&lineBuf[0], typeBodyBoundary) ) {
		
		return;
	}
	
	//otherwise it must be either a function or a stateNode
	//get the .name of the node
	int lineBufPos = 0;
	while (true) {
		inc_namePos();
		
		//check for newlines and the typeBodyBoundary
		if (lineBuf[lineBufPos] == '\0') {
			nodes[currentNode].name[namePos] = '\n';
			getLine();
			lineBufPos = -1;
			if (lineBuf[0] == '\t') {
				if (lineBuf[1] == typeBodyBoundary) {
					if (lineBuf[2] != '\0') 
						putError("unexpected text\n");
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
		
		lineBufPos++;
	}
	
	//stateNode
	if (inFrameform) {
		inc_currentStateNode();
		
		//the current stateNode is the current node
		nodes[currentNode].evaluate = eval_state;
		frameforms[currentFrameform].stateNodes[
			frameforms[currentFrameform].currentStateNode
		] = currentNode;
	}
	//rootNode function
	else {
		
	}
	
	//check for naming collisions
	
	
	//the next node must be the body of this defNode
	getArgs();
	
}
void  getRefNode(void) {
	inc_currentNode();
	
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
			//it's a match
			nodes[currentNode] = *stdNodeTable[i];
			return;
		}
	}
	
	//check for reference to a rootNode
	for (int i = 0; i < currentRootNode; i++) {
		int charIndex = 0;
		for (
			;
			lineBuf[charIndex] == nodes[ rootNodes[i] ].name[charIndex];
			charIndex++
		) {}
		if (
			lineBuf[charIndex] == '\0' &&
			nodes[ rootNodes[i] ].name[charIndex] == '\n'
		) {
			//it's a match
			nodes[currentNode] = nodes[ rootNodes[i] ];
			nodes[currentNode].definition = rootNodes[i];
			
			//is it a function?
			if ( nodes[ rootNodes[i] ].evaluate == eval_fnDef )
				nodes[currentNode].evaluate = eval_fnCall;
			//or a variable?
			else
				nodes[currentNode].evaluate = eval_varCall;
			
			return;
		}
	}
	
	//check for reference to a stateNode
	
	
	//undefined for now
	
	
	//temporary
	putError("not recognized: "); printf("%s\n", lineBuf);
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
		getRefNode();
		nodes[parent].arguments[argIndex] = currentNode;
		if (nodes[currentNode].arity) {
			getArgs();
		}
	}
	
	expectedIndentation--;
}


void  getFrameform(void) {
	inc_currentFrameform();
	inFrameform = true;	
	
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
	int ffNamePos = 0;
	while (true) {
		frameforms[currentFrameform].name[ffNamePos] = lineBuf[bufPos];
		if (lineBuf[bufPos] == '\0')
			break;
		ffNamePos++;
		bufPos++;
	}
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
		
		//we must have a DefNode
		else
			getDefNode();
	}
	
	//recheck undefined
	
	//check for type errors
	
}

