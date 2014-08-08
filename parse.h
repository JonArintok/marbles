

#define commentInitChar   '|'
#define frameformInitChar '#'
#define paramTypeInitChar '<'

FILE    *fileStream;
char    *fileName;
char     lineBuf[maxLineLength];
uint8_t  expectedIndentation = 0;
uint32_t currentLine = 0;

bool reachedEOF  = false;
bool noErrors    = true;
bool inFrameform = false;

void putError(char *message) {
	noErrors = false;
	printf(
		"error in line %d of %s: %s", 
		currentLine, fileName, message
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
			putError("line is too long\n");
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
				putError("leading space\n");
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
				putError("leading character: '");
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
			putError("unexpected tab character\n");
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
	
	//read the first line into currentNode.name
	while (true) {
		incNamePos();
		
		//check for number literal
		if (
			lineBuf[namePos] == ' ' &&
			isNumeric( lineBuf[namePos+1] )
		) {
			int bufPos = namePos;
			nodes[currentNode].name[namePos] = '\0';
			nodes[currentNode].evaluate = eval_varDef;
			inc_currentNode();
			while (lineBuf[bufPos] != '\0') {
				incNamePos();
				bufPos++;
				nodes[currentNode].name[namePos] = lineBuf[bufPos];
			}
			return;
		}
		
		nodes[currentNode].name[namePos] = lineBuf[namePos];
		
		if (lineBuf[namePos] == '\0')
			break;
	}
	
	//else getParamCount, if paramCount > maxArity then error
	//	if paramCount == 0 then check to see if it's a var or nullary fn
	//		if ( strcmp(&lineBuf[namePos-7], "nullary") )
	//		getArgs
	//	else swap that /0 with a /n and continue reading into the name
	//for each param, getLine and read it into currentNode.name
	//getArgs
	
	
}

void getRefnode(char *title) {}

void getFrameform(void) {
	inc_currentFrameform();
	inFrameform = true;	
	
	//check for proper frameform name
	if (lineBuf[1] == '\0') {
		putError("This frameform must have a name\n");
		return;
	}
	else if ( isNumeric( lineBuf[1] ) ) {
		putError(
			"frameform names may not begin with numeric characters\n"
		);
		return;
	}
	//allow no spaces in the frameform name
	if ( strchr(&lineBuf[1], ' ') ) {
		putError("frameform names may not contain spaces\n");
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
		else if (lineBuf[0] == frameformInitChar) {
			if (lineBuf[1] == frameformInitChar)
				inFrameform = false;
			else
				getFrameform();
		}
		
		//we must have a DefNode
		else
			getDefnode();
	}
	
	//recheck undefined
	
	//check for type errors
	
}
