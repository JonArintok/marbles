

#define maxTokenLength  120
#define rootNodePageSize  20
#define commentChar  '/'

FILE *fileStream;
char  fileChar;
char  tokenBuf[maxTokenLength];
uint  expectedIndentation;
uint  currentLine = 1;

int   reachedEOF = 0;
int   noErrors = 1;


node *lookupNode(char *nameIn) {
	uint i=0;
	for (; i<stdNodeTableLength; i++)
		if (!( strcmp(stdNodeTable[i].name, nameIn) ))
			return &stdNodeTable[i];
	noErrors = 0;
	return NULL;
}

void  getArgs(node *parent);

void getNode(node *destination) {
	
	uint tokenCharIndex = 0;
	for (;; tokenCharIndex++) {
		if (tokenCharIndex == maxTokenLength) {
			noErrors = 0;
			printf(
				"error: token at line %d exceeds maxTokenLength\n", 
				currentLine
			);
			return;
		}
		
		fileChar = fgetc(fileStream);
		
		if (!tokenCharIndex  &&  fileChar == ' ') {
			noErrors = 0;
			printf("error: leading space at line %d\n", currentLine);
			return;
		}
		else if (fileChar == EOF) {
			reachedEOF = 1;
			if (expectedIndentation > 0) {
				noErrors = 0;
				printf("error: file ended unexpectadly at line %d\n", currentLine);
			}
			return;
		}
		else if (fileChar == '\t') {
			noErrors = 0;
			printf("error: line %d is over-indented\n", currentLine);
			return;
		}
		else if (fileChar == '\n'  ||  fileChar == commentChar) {
			//terminate the token
			tokenBuf[tokenCharIndex] = '\0';
			
			//remove trailing whitespace, if any
			if (tokenCharIndex > 1) {
				uint backstep = 1;
				while (tokenBuf[tokenCharIndex-backstep] == ' ') {
					tokenBuf[tokenCharIndex-backstep] = '\0';
					backstep++;
				}
			}
			
			//if we hit a comment, read through the rest of the comment
			if (fileChar == commentChar) {
				for (;; tokenCharIndex++) {
					fileChar = fgetc(fileStream);
					if (fileChar == '\n')
						break;
					if (fileChar == EOF) {
						reachedEOF = 1;
						if (expectedIndentation > 0) {
							noErrors = 0;
							printf("error: file ended unexpectadly at line %d\n", currentLine);
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
			noErrors = 0;
			printf("error: expected an argument at line %d\n", currentLine-1);
		}
		return;
	}
	
	node *nodeToGet;
	
	if (tokenBuf[0] >= '0'  &&  tokenBuf[0] <= '9') {
		//make sure it's a valid number
		for (
			tokenCharIndex=0; 
			tokenCharIndex < maxTokenLength; 
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
				noErrors = 0;
				printf(
					"error: invalid number literal '%s' at line %d\n",
					tokenBuf, currentLine-1
				);
				return;
			}
		}
		
		*destination = node_numLit;
		sscanf(tokenBuf, "%lf", &destination->output.n);
		return;
	}
	
	nodeToGet = lookupNode(tokenBuf);
	
	if (nodeToGet == NULL) {
		noErrors = 0;
		printf(
			"error: '%s' at line %d is not recognized\n", 
			tokenBuf, currentLine
		);
		return;
	}
	
	*destination = *nodeToGet;
	if (destination->arity) {
		destination->arguments = malloc(sizeof(node)*destination->arity);
		getArgs(destination);
	}
	
}

void getArgs(node *parent) {
	expectedIndentation++;
	uint indentation = 0;
	uint currentArg = 0;
	for (; currentArg < parent->arity; currentArg++) {
		indentation = 0;
		while (indentation < expectedIndentation) {
			fileChar = fgetc(fileStream);		
			if (fileChar == '\t') 
				indentation++;
			else if (fileChar == EOF) {
				reachedEOF = 1;
				printf("error: file ended unexpectadly at line %d\n", currentLine);
				return;
			}
			else {
				printf("error: line %d, under-indented\n", currentLine);
				return;
			}
		}
		getNode(&parent->arguments[currentArg]);
		//check that the node is of the requiredType
	}
	expectedIndentation--;
}
