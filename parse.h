

#define commentInitChar   '|'
#define frameformInitChar '#'
#define paramTypeInitChar '<'

FILE    *fileStream;
char    *fileName;
char     lineBuf[maxLineLength];

bool reachedEOF  = false;
bool noErrors    = true;
bool inFrameform = false;

void putError(char *message, int line) {
	noErrors = false;
	printf(
		"error in line %d of %s: %s", 
		line, fileName, message
	);
}

int isNumeric(char in) {
	return  (in >=  '0' && in <=  '9') || in == '-' || in == '.';
}

void getLine(void) {
	currentLine++;
	
	//read the next line from the file one character at a time
	for (int bufPos = 0;; bufPos++) {
		char fileChar;
		
		//check to be sure we are not overflowing the buffer
		if (bufPos == maxLineLength) {
			putError("line is too long\n", currentLine);
			return;
		}
		
		//get character from file
		fileChar = fgetc(fileStream);
		
		//check for EOF
		if (fileChar == EOF) {
			reachedEOF = true;
			break;
		}
		
		//homogenize formatting:
		//ignore redundant spaces,
		//ignore spaces around parentheses
		//ignore spaces around paramTypeInitChar
		//ignore spaces after frameformInitChar
		if (fileChar == ' ') {
			if (!bufPos) {
				putError("leading space\n", currentLine);
				return;
			}
			if (
				lineBuf[bufPos-1] == ' ' ||
				lineBuf[bufPos-1] == '(' ||
				lineBuf[bufPos-1] == ')' ||
				lineBuf[bufPos-1] == frameformInitChar ||
				lineBuf[bufPos-1] == paramTypeInitChar
			) {
				bufPos--;
				continue;
			}
		}
		if (
			fileChar == '(' ||
			fileChar == ')' ||
			fileChar == paramTypeInitChar
		) {
			if (!bufPos) {
				putError("leading character: '", currentLine);
				printf("%c", fileChar);
				puts("' is not allowed\n");
				return;
			}
			if (lineBuf[bufPos-1] == ' ') {
				bufPos--;
				lineBuf[bufPos] = fileChar;
				continue;
			}
		}
		//tab characters are for indentation only
		if (bufPos && fileChar == '\t' && lineBuf[bufPos-1] != '\t') {
			putError("unexpected tab character\n", currentLine);
			return;
		}
		
		lineBuf[bufPos] = fileChar;
		
		//ignore comments
		if (fileChar == commentInitChar) {
			//end of the line
			if (bufPos && lineBuf[bufPos-1] == ' ')
				lineBuf[bufPos-1] = '\0';
			else
				lineBuf[bufPos] = '\0';
			//read through to the newline character
			while ( (fileChar = fgetc(fileStream)) != '\n' )
				if (fileChar == EOF) 
					reachedEOF = true;
			break;
		}
		
		//check for newline character
		if (fileChar == '\n') {
			//end of the line
			lineBuf[bufPos] = '\0';
			break;
		}
	}
}

void getDefnode(void) {
	//"Defnodes" are variables, functions, and statenodes
	inc_currentNode();
	nodes[currentNode].arity = 1;
	nodes[currentNode].arguments[0] = currentNode + 1;
	nodeLines[currentNode] = currentLine;
	nodeLevels[currentnode] = 0;
	
	//read the first line into currentNode.name
	while (true) {
		inc_namePos();
		//check for number literal
		if (
			lineBuf[namePos] == ' ' &&
			isNumeric( lineBuf[namePos+1] )
		) {
			int bufPos = namePos;
			nodes[currentNode].name[namePos] = '\0';
			nodes[currentNode].evaluate = eval_varDef;
			inc_currentNode();
			nodeLines[currentNode] = currentLine;
			nodeLevels[currentnode] = 1;
			while (lineBuf[bufPos] != '\0') {
				inc_namePos();
				bufPos++;
				nodes[currentNode].name[namePos] = lineBuf[bufPos];
			}
			return;
		}
		nodes[currentNode].name[namePos] = lineBuf[namePos];
		if (lineBuf[namePos] == '\0')
			break;
	}
	
	//find out how many parameters there are
	int paramCount = 0;
	fpos_t filePos;
	fgetpos(fileStream, &filePos);
	while (true) {
		char fileChar;
		int  indent = 0;
		while ( (fileChar = fgetc(fileStream)) == '\t')
			indent++;
		if (indent == 1)
			paramCount++;
		else {
			paramCount--;
			fsetpos(fileStream, &filePos);
			break;
		}
		while ( (fileChar = fgetc(fileStream)) != '\n') {
			if (fileChar == EOF) {
				reachedEOF = true;
				putError("incomplete definition\n", currentLine);
				break;
			}
		}
	}
	
	//determine if it's a stateDef, varDef, or fnDef
	if ( !paramCount && strcmp(&lineBuf[namePos-7], "nullary") ) {
		if (inFrameform)
			nodes[currentNode].evaluate = eval_stateDef;
		else
			nodes[currentNode].evaluate = eval_varDef;
	}
	else {
		nodes[currentNode].evaluate = eval_fnDef;
		//else replace that \0 with a \n and continue reading the name
		for (int i = 0; i < paramCount; i++) {
			nodes[currentNode].name[namePos] = '\n';
			getLine();
			int bufPos = 0;//would be -1 if we weren't ignoring the tabs
			while (lineBuf[bufPos] != '\0') {
				inc_namePos();
				bufPos++;
				nodes[currentNode].name[namePos] = lineBuf[bufPos];
			}
		}
	}
	//initialize and name the body's nodes
	int elevation;
	int fold  = 0;
	int bufPos;
	char prevDelim = '\0';
	while (true) {
		getLine();
		bufPos = -1;
		elevation = 0;
		while (lineBuf[++bufPos] = '\t')
			elevation++;
		if (!elevation)
			break;
		inc_currentNode();
		bufPos--;
		while (lineBuf[bufPos++] != '\0') {
			inc_namePos();
			switch (lineBuf[bufPos]) {
				case ' ':
					if (prevDelim != ' ')
						fold++;
					nodes[currentNode].name[namePos] = '\0';
					nodeLines[currentNode] = currentLine;
					nodeLevels[currentnode] = elevation + fold;
					prevDelim == ' ';
					break;
				case '(':
					fold++;
					if (lineBuf[bufPos-1] != ')') {
						nodes[currentNode].name[namePos] = '\0';
						nodeLines[currentNode] = currentLine;
						nodeLevels[currentnode] = elevation + fold;
					}
					prevDelim == '(';
					break;
				case ')':
					if (prevDelim == '(') {
						putError(
							"unnecessary perentheses around '",
							currentLine
						);
						nodes[currentNode].name[namePos] = '\0';
						printf("%s'\n", nodes[currentNode].name);
						return;
					}
					else {
						if (lineBuf[bufPos-1] != ')') {
							nodes[currentNode].name[namePos] = '\0';
							nodeLines[currentNode] = currentLine;
							nodeLevels[currentnode] = elevation + fold;
						}
						fold--;
					}
					prevDelim == ')';
					break
				case '\0':
					if (fold) {
						putError("perentheses are off by: ", currentLine);
						printf("%d\n", fold);
						return;
					}
					else
						if (lineBuf[bufPos-1] != ')') {
							nodes[currentNode].name[namePos] = '\0';
							nodeLines[currentNode] = currentLine;
							nodeLevels[currentnode] = elevation + fold;
						}
					prevDelim == '\0';
					break;
				default:
					nodes[currentNode].name[namePos] = lineBuf[bufPos];
			}
		}
	}
}

void connectNodes(nodeIndex nodeIn) {}

void getFrameform(void) {
	inc_currentFrameform();
	inFrameform = true;	
	
	//check for proper frameform name
	if (lineBuf[1] == '\0') {
		putError("This frameform must have a name\n", currentLine);
		return;
	}
	else if ( isNumeric( lineBuf[1] ) ) {
		putError(
			"frameform names may not begin with numeric characters\n",
			currentLine
		);
		return;
	}
	//allow no spaces in the frameform name
	if ( strchr(&lineBuf[1], ' ') ) {
		putError("frameform names may not contain spaces\n", currentLine);
		return;
	}
	
	//copy from lineBuf to frameforms[currentFrameform].name
	strncpy(
		&frameforms[currentFrameform].name[0],
		&lineBuf[1],
		maxTokenLength
	);
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
		if (lineBuf[0] == frameformInitChar) {
			if (lineBuf[1] == frameformInitChar)
				inFrameform = false;
			else
				getFrameform();
		}
		//we must have some nodes
		else
			initNodes();
	}
	
	//
	
	//check for type errors
	
}
