

#define commentInitChar   '|'
#define frameformInitChar '#'
#define bodyInitChar      '='
#define fnOutTypeInitChar '>'

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
				while (lineBuf[lineCharIndex-1] == ' ') {
					lineCharIndex--;
					lineBuf[lineCharIndex] = '\0';
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
		else if (lineBuf[0] == frameFormInitializer) {
			if (lineBuf[1] == frameFormInitializer)
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
