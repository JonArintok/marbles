

FILE     *fileStream;
char      fileChar;
#define   maxTokenLength 80
char      tokenBuf[maxTokenLength];
uint8_t   expectedIndentation;
uint32_t  currentLine = 1;

bool      reachedEOF = false;
bool      noErrors = true;

nodeIndex currentNode = 0;
#define   rootNodePageSize 20
nodeIndex rootNodes[rootNodePageSize];
int16_t   currentRootNode = 0;

#define commentChar  '`'


int  lookupNode(char *nameIn) {
	for (int i=0; i<stdNodeTableLength; i++)
		if (!( strcmp(stdNodeTable[i]->name, nameIn) ))
			return i;
	noErrors = false;
	return -1;
}

void  getNode() {
	
	int tokenCharIndex = 0;
	for (;; tokenCharIndex++) {
		if (tokenCharIndex == maxTokenLength) {
			noErrors = false;
			printf(
				"error: token at line %d exceeds maxTokenLength\n", 
				currentLine
			);
			return;
		}
		
		fileChar = fgetc(fileStream);
		
		if (!tokenCharIndex  &&  fileChar == ' ') {
			noErrors = false;
			printf("error: leading space at line %d\n", currentLine);
			return;
		}
		else if (fileChar == EOF) {
			reachedEOF = true;
			if (expectedIndentation > 0) {
				noErrors = false;
				printf("error: file ended unexpectadly at line %d\n", currentLine);
			}
			return;
		}
		else if (fileChar == '\t') {
			noErrors = false;
			printf("error: line %d is over-indented\n", currentLine);
			return;
		}
		else if (fileChar == '\n'  ||  fileChar == commentChar) {
			//null terminate the token
			tokenBuf[tokenCharIndex] = '\0';
			
			//remove trailing whitespace, if any
			if (tokenCharIndex > 1) {
				int backstep = 1;
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
						reachedEOF = true;
						if (expectedIndentation > 0) {
							noErrors = false;
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
			noErrors = false;
			printf("error: expected an argument at line %d\n", currentLine-1);
		}
		return;
	}
	
	//check for number literal
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
	
	nodes[currentNode] = *stdNodeTable[nodeToGet];
	
	
	// get arguments, if any
	if (nodes[currentNode].arity) {
		nodeIndex parent = currentNode;
		currentNode++;
		expectedIndentation++;
		int indentation = 0;
		nodeIndex currentArg = 0;
		for (; currentArg < nodes[parent].arity; currentArg++) {
			indentation = 0;
			while (indentation < expectedIndentation) {
				fileChar = fgetc(fileStream);		
				if (fileChar == '\t') 
					indentation++;
				else if (fileChar == EOF) {
					reachedEOF = true;
					printf("error: file ended unexpectadly at line %d\n", currentLine);
					return;
				}
				else {
					printf("error: line %d, under-indented\n", currentLine);
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


