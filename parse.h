

#define   commentInitializer   '|'
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

int  spacerCount(char *in) {
	int  count = 0;
	int  pos = -1;
	while (true) {
		pos++;
		if ( in[pos] == ' '  &&  in[pos+1] == ' ' ) {
			count++;
			while (in[pos] == ' ')
				pos++;
		}
		if (in[pos+1] == '\0')
			return count;
	}
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
			lineBuf[lineBufIndex] = 
				lineBuf[ expectedIndentation + lineBufIndex ];
		}
	}
	
}


bool  matchHeteroTerm(char *nullTerm, char *newlineTerm) {
	int charIndex = 0;
	while (nullTerm[charIndex] == newlineTerm[charIndex])
		charIndex++;
	if (
		nullTerm[charIndex] == '\0' &&
		newlineTerm[charIndex] == '\n'
	) 
		return true;
	else
		return false;
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
		if ( matchHeteroTerm(&lineBuf[0], stdNodeTable[i]->name) ) {
			//it's a match
			nodes[currentNode] = *stdNodeTable[i];
			return;
		}
	}
	
	//check for stateCall
	for (int ffi = 0; ffi <= currentFrameform; ffi++) {
		for (int sni = 0; sni <= frameforms[ffi].currentStateNode; sni++) {
			if (
				matchHeteroTerm(
					&lineBuf[0],
					nodes[ frameforms[ffi].stateNodes[sni] ].name
				)
			) {
				//it's a match
				nodes[currentNode].name = nodes[
					frameforms[ffi].stateNodes[sni]
				].name;
				nodes[currentNode].arity = 0;
				nodes[currentNode].definition = frameforms[ffi].stateNodes[sni];
				nodes[currentNode].evaluate = eval_stateCall;
				return;
			}
		}
	}
	
	//check for fnCall or varCall
	for (int i = 0; i <= currentRootNode; i++) {
		if ( matchHeteroTerm(&lineBuf[0], nodes[ rootNodes[i] ].name) ) {
			//it's a match
			nodes[currentNode] = nodes[ rootNodes[i] ];
			
			//is it a function?
			if ( nodes[ rootNodes[i] ].evaluate == eval_fnDef ) {
				nodes[currentNode].arity = spacerCount( nodes[ rootNodes[i] ].name );
				nodes[currentNode].evaluate = eval_fnCall;
			}
			//or a variable?
			else {
				nodes[currentNode].definition = rootNodes[i];
				nodes[currentNode].arity = 0;
				nodes[currentNode].evaluate = eval_varCall;
			}
			
			return;
		}
	}
	
	//check for argCall
	if (
		nodes[currentRootNode].evaluate  ==  eval_fnDef  &&
		nodes[currentRootNode].arity  >  0
	) {
		int argPos  = -1;
		int namePos = -1;
		while (true) {
			namePos++;
			if (
				nodes[currentRootNode].name[namePos  ] == ' '  &&
				nodes[currentRootNode].name[namePos+1] == ' '  &&
				nodes[currentRootNode].name[namePos+2] != ' '
			) {
				argPos++;
				if (
					matchHeteroTerm(
						&lineBuf[0],
						&nodes[currentRootNode].name[namePos+2]
					)
				) {
					//it's a match
					nodes[currentNode].definition = currentRootNode;
					nodes[currentNode].argRefIndex = argPos;
					nodes[currentNode].evaluate = eval_argCall;
					
					//this node will also need the .name for type checking
					
					return;
				}
				//check if this is the last argument
				if (argPos  ==  nodes[currentRootNode].arity - 1)
					break;
			}
		}
	}
	
	
	
	//undefined for now, we'll check again on the second pass
	
	
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


void  getDefNode(void) {
	
	//could be rootNode or stateNode, but either way...
	inc_currentNode();
	nodes[currentNode].arity = 1;
	nodes[currentNode].arguments[0] = currentNode + 1;
	
	int  titleLineSpacerCount = spacerCount(&lineBuf[0]);
	
	if (titleLineSpacerCount == 1  ||  titleLineSpacerCount > 2) {
		putError("tokens may not contain spacers\n");
		return;
	}
	
	//variable
	if (titleLineSpacerCount == 2) {
		//get the .name and value of the variable
		
		//get the title on the first loop, output type on the second
		int  lineBufPos = -1;
		for (int firstLoop = 1; firstLoop > -1; firstLoop--) {
			
			while (true) {
				inc_namePos();
				lineBufPos++;
				nodes[currentNode].name[namePos] = lineBuf[lineBufPos];
				if (
					lineBuf[lineBufPos+1] == ' ' &&
					lineBuf[lineBufPos+2] == ' '
				) {
					break;
				}
			}
			
			if (firstLoop) {
				//make the name consistant with functions and stateNodes
				inc_namePos();
				nodes[currentNode].name[namePos] = '\n';
				inc_namePos();
				nodes[currentNode].name[namePos] = '\t';
			}
			else {
				//terminate the name
				inc_namePos();
				nodes[currentNode].name[namePos] = '\0';
			}
			
			//read through the spacer, however long it may be
			while (lineBuf[lineBufPos+1] == ' ') {
				lineBufPos++;
			}
		}
		
		
		//move the value up so that it's the only thing in lineBuf
		int offset = lineBufPos + 1;
		while (true) {
			lineBufPos++;
			lineBuf[lineBufPos - offset] = lineBuf[lineBufPos];
			if (lineBuf[lineBufPos] == '\0')
				break;
		}
		
		//add the root node
		inc_currentRootNode();
		rootNodes[currentRootNode] = currentNode;
		nodes[currentNode].evaluate = eval_varDef;
		
		//the next node should be a number literal
		getRefNode();
		
		return;
	}
	//otherwise it must be either a function or a stateNode
	else {
		
		//get the .name of the function or a stateNode
		
		int  lineBufPos = -1;
		int  titleLine  = currentLine;
		while (true) {
			inc_namePos();
			lineBufPos++;
			
			//ignore spaces beyond 2 in a row
			while (
				lineBuf[lineBufPos  ] == ' ' &&
				lineBuf[lineBufPos+1] == ' ' &&
				lineBuf[lineBufPos+2] == ' '
			) {
				lineBufPos++;
			}
			
			//check for newlines
			if (lineBuf[lineBufPos] == '\0') {
				nodes[currentNode].name[namePos] = '\n';
				if (
					currentLine != titleLine &&
					!( spacerCount(&lineBuf[0]) ) &&
					lineBuf[1] != '\t'
				) {
					//that's the end of the type declaration
					break;
				}
				//otherwise, on to the next line of the type declaration
				getLine();
				lineBufPos = -1;
				if (lineBuf[0] != '\t') {
					putError("incomplete type declaration\n");
					return;
				}
			}
			//else copy the char
			else
				nodes[currentNode].name[namePos] = lineBuf[lineBufPos];
		}
	}
	
	//stateNode
	if (inFrameform) {
		inc_currentStateNode();
		
		//the current stateNode is the current node
		nodes[currentNode].evaluate = eval_stateDef;
		frameforms[currentFrameform].stateNodes[
			frameforms[currentFrameform].currentStateNode
		] = currentNode;
		
		//initialize output (eventually this will need to be type-aware)
		nodes[currentNode].output.n = 0;
	}
	//rootNode function
	else {
		inc_currentRootNode();
		rootNodes[currentRootNode] = currentNode;
		nodes[currentNode].evaluate = eval_fnDef;
	}
	
	
	//check for naming collisions
	
	
	//the next node must be the body of this defNode
	getArgs();
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

