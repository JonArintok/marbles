

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

void getLine(void) {
	currentLine++;
	
	//read the next line from the file one char at a time
	for (int bufPos = 0;; bufPos++) {
		char fileChar;
		
		//check to be sure we are not overflowing the buffer
		if (bufPos == maxLineLength) {
			putError("line is too long\n");
			return;
		}
		
		//get char from file
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
		if (fileChar == ' ') {
			if (!bufPos) {
				putError("leading space\n");
				return;
			}
			if (
				lineBuf[bufPos-1] == ' ' ||
				lineBuf[bufPos-1] == '(' ||
				lineBuf[bufPos-1] == ')' ||
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
		//tab chars are for indentation only
		if (bufPos && fileChar == '\t' && lineBuf[bufPos-1] != '\t') {
			putError("unexpected tab character\n");
			return;
		}
		
		lineBuf[bufPos] = fileChar;
		
		//check for newlines and comments, remove trailing whitespace
		if (
			fileChar == '\n' ||
			fileChar == commentInitChar
		) {
			//if we're not at the end of the line, read through to the end
			if (fileChar != '\n') {
				char c;
				while ( ( c = fgetc(fileStream) ) != '\n' ) {
					if (c == EOF) {
						reachedEOF = true;
						break;
					}
				}
			}
			//null terminate the string
			lineBuf[bufPos] = '\0';
			//remove trailing whitespace, if any
			if (bufPos > 1) {
				while (lineBuf[bufPos-1] == ' ') {
					bufPos--;
					lineBuf[bufPos] = '\0';
				}
			}
			//end of the line
			break;
		}
	}
}


void getRefnode(char *title) {}

void getDefnode(void) {
	//Defnodes are variables, functions, and statenodes
	inc_currentNode();
	nodes[currentNode].arity = 1;
	nodes[currentNode].arguments[0] = currentNode + 1;
	
	
}

void getFrameform(void) {
	inc_currentFrameform();
	inFrameform = true;	
	
	//ignore spaces between the '#' and the name
	int bufPos = 1;
	while (true) {
		if (lineBuf[bufPos] == ' ')
			bufPos++;
		else if (lineBuf[bufPos] == '\0') {
			putError("This frameform must have a name\n");
			return;
		}
		else if (lineBuf[bufPos] >=  '0'  &&  lineBuf[bufPos] <=  '9') {
			putError("frameform names may not begin with numerals\n");
			return;
		}
		else break;
	}
	//allow no spaces in the frameform name
	if ( strchr(&lineBuf[bufPos], ' ') ) {
		putError("frameform names may not contain spaces\n");
		return;
	}
	
	//copy from lineBuf to frameforms[currentFrameform].name
	strncpy(
		&frameforms[currentFrameform].name[0],
		&lineBuf[bufPos],
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
