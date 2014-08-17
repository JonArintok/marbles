

#define commentInitChar   '/'
#define frameformInitChar '#'
#define paramTypeInitChar '<'

FILE    *fileStream;
char    *fileName;
char     lineBuf[maxLineLength];

bool reachedEOF  = false;
bool inFrameform = false;
int  errorCount  = 0;

void putError(int line, char *message) {
	errorCount++;
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
			putError(currentLine, "line is too long\n");
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
				putError(currentLine, "leading space\n");
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
				putError(currentLine, "leading character '");
				printf("%c' is not allowed\n", fileChar);
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
			putError(currentLine, "unexpected tab character\n");
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

bool matchStrXDelim(char *A, char AD, char *B, char BD) {
	//return true if the strings (A and B) match up until their 
	//respective delimeters (AT and BT)
	char a;
	char b;
	for (int i = 0;; i++) {
		a = A[i];
		b = B[i];
		if (a != b || a == AD || b == BD || a == '\0' || b == '\0') {
			if (a == AD && b == BD)
				return true;
			return false;
		}
	}
}

int findNameCollision(char *nameIn, char AD) {
	
	#define _putNameCollisionError_\
		putError(currentLine, "name collision on '");\
		printf("%s'\n", nameIn);\
		return 1;
	
	//check for name collision with stdNodes
	for (int i = 0; i < stdNodeTableLength; i++) {
		if (matchStrXDelim(
			nameIn, AD, stdNodeTable[i]->name, ' '
		)) {
			_putNameCollisionError_
		}
	}
	//check for name collision with rootNodes
	for (int i = 0; i <= currentRootNode; i++) {
		if (matchStrXDelim(
			nameIn, AD, nodesInfo[ rootNodes[i] ].name, ' '
		)) {
			_putNameCollisionError_
		}
	}
	//check for name collision with stateNodes
	if (inFrameform) {
		//check for collision with stateNodes in the currentFrameform
		for (
			int i = 0;
			i <= frameforms[currentFrameform].currentStateNode;
			i++
		) {
			if (matchStrXDelim(
				nameIn, AD,
				nodesInfo[ frameforms[currentFrameform].stateNodes[i] ].name, ' '
			)) {
				_putNameCollisionError_
			}
		}
	}
	else if (currentFrameform >= 0) {
		//check for collision with any stateNode
		for (
			int ffi;
			ffi <= currentFrameform;
			ffi++
		) {
			for (
				int sni = 0;
				sni <= frameforms[ffi].stateNodes[sni];
				sni++
			) {
				if (matchStrXDelim(
					nameIn, AD,
					nodesInfo[ frameforms[ffi].stateNodes[sni] ].name, ' '
				)) {
					_putNameCollisionError_
				}
			}
			//check for collision with frameform names
			if (matchStrXDelim(
				nameIn, AD,
				frameforms[ffi].name, '\0'
			)) {
				_putNameCollisionError_
			}
		}
	}
	
	return 0;
}

void initNodes(void) {
	//"defNodes" are variables, functions, and stateNodes
	inc_currentNode();
	nodes[currentNode].childCount = 1;
	nodes[currentNode].children[0] = currentNode + 1;
	nodesInfo[currentNode].line = currentLine;
	nodesInfo[currentNode].level = 0;
	
	//read the first line into currentNode.name
	while (true) {
		inc_namePos();
		//check for number literal constant
		if (
			lineBuf[namePos] == ' ' &&
			isNumeric( lineBuf[namePos+1] )
		) {
			int bufPos = namePos;
			nodesInfo[currentNode].name[namePos] = '\0';
			nodes[currentNode].evaluate = eval_varDef;
			inc_currentRootNode();
			rootNodes[currentRootNode] = currentNode;
			inc_currentNode();
			nodesInfo[currentNode].line = currentLine;
			nodesInfo[currentNode].level = 1;
			while (lineBuf[bufPos] != '\0') {
				inc_namePos();
				bufPos++;
				nodesInfo[currentNode].name[namePos] = lineBuf[bufPos];
			}
			return;
		}
		nodesInfo[currentNode].name[namePos] = lineBuf[namePos];
		if (lineBuf[namePos] == '\0')
			break;
	}
	
	
	char *nodeName = nodesInfo[currentNode].name;
	
	//if ( findNameCollision(nodeName, ' ') )
	//	return;
	
	//find out how many parameters there are
	int paramCount = 0;
	fpos_t filePos;
	fgetpos(fileStream, &filePos);
	while (true) {
		int  indent = 0;
		while (fgetc(fileStream) == '\t')
			indent++;
		if (indent == 1)
			paramCount++;
		else {
			paramCount--;
			fsetpos(fileStream, &filePos);
			break;
		}
		char fileChar;
		while ( (fileChar = fgetc(fileStream)) != '\n') {
			if (fileChar == EOF) {
				reachedEOF = true;
				putError(currentLine, "incomplete definition\n");
				break;
			}
		}
	}
	nodesInfo[currentNode].paramCount = paramCount;
	
	//determine if it's a stateDef, varDef, or fnDef
	if ( !paramCount && strcmp(&lineBuf[namePos-7], "nullary") ) {
		if (inFrameform) {
			nodes[currentNode].evaluate = eval_stateDef;
			inc_currentStateNode();
			frameforms[currentFrameform].stateNodes[
				frameforms[currentFrameform].currentStateNode
			] = currentNode;
		}
		else
			nodes[currentNode].evaluate = eval_varDef;
	}
	else {
		nodes[currentNode].evaluate = eval_fnDef;
		//else replace that \0 with a \n and continue reading the name
		for (int i = 0; i < paramCount; i++) {
			nodeName[namePos] = '\n';
			getLine();
			int bufPos = 0;//would be -1 if we weren't ignoring the tabs
			while (lineBuf[bufPos] != '\0') {
				inc_namePos();
				bufPos++;
				nodeName[namePos] = lineBuf[bufPos];
			}
		}
	}
	/*
	//update the frameform or rootNode array
	if (nodes[currentNode].evaluate == eval_stateDef) {
		inc_currentStateNode();
		frameforms[currentFrameform].stateNodes[
			frameforms[currentFrameform].currentStateNode
		] = currentNode;
	}
	else {
		inc_currentRootNode();
		rootNodes[currentRootNode] = currentNode;
	}*/

	
	//initialize the nodes in the body of the defNode
	while (true) {
		int elevation = 0;
		int fold      = 0;
		int bufPos    = 0;
		char prevDelim = '\n';
		
		#define _setNodesInfo_\
			inc_namePos();\
			nodesInfo[currentNode].name[namePos] = '\0';\
			nodesInfo[currentNode].level = elevation + fold;\
			nodesInfo[currentNode].line = currentLine;\
			nodesInfo[currentNode].frameform = inFrameform ? currentFrameform : -1;
		
		getLine();
		for (; lineBuf[bufPos] == '\t'; bufPos++)
			elevation++;
		if (!elevation)
			break;
		inc_currentNode();
		for (; prevDelim != '\0'; bufPos++) {
			switch (lineBuf[bufPos]) {
				case ' ':
					_setNodesInfo_
					inc_currentNode();
					if (prevDelim != ' ')
						fold++;
					prevDelim = ' ';
					break;
				case '(':
					if (lineBuf[bufPos-1] != ')') {
						_setNodesInfo_
						inc_currentNode();
					}
					if (prevDelim != ' ')
						fold++;
					prevDelim = '(';
					break;
				case ')':
					if (prevDelim == '(') {
						inc_namePos();
						nodesInfo[currentNode].name[namePos] = '\0';
						putError(currentLine, "superfluous perentheses around '");
						printf("%s'\n", nodesInfo[currentNode].name);
						return;
					}
					if (lineBuf[bufPos-1] != ')') {
						_setNodesInfo_
					}
					if (lineBuf[bufPos+1] != '\0' && lineBuf[bufPos+1] != ')') {
						inc_currentNode();
					}
					if (prevDelim == ' ')
						fold--;
					fold--;
					prevDelim = ')';
					break;
				case '\0':
					if (fold && prevDelim != ' ') {
						putError(currentLine, "perentheses are off by ");
						printf("%d\n", fold);
						return;
					}
					else
						if (lineBuf[bufPos-1] != ')') {
							_setNodesInfo_
						}
					prevDelim = '\0';
					break;
				default:
					inc_namePos();
					nodesInfo[currentNode].name[namePos] = lineBuf[bufPos];
			}
		}
	}
}

void connectNode(nodeIndex nodePos) {	
	//nodes at levels 0 and 1 should already be connected
	if (nodesInfo[nodePos].level < 2)
		return;
	//look backwards for the node's parent
	for (nodeIndex backNode = nodePos - 1;; backNode--) {
		if (
			nodesInfo[backNode].level == 
			nodesInfo[nodePos].level - 1
		) {
			nodes[backNode].children[ nodes[backNode].childCount ] = nodePos;
			nodes[backNode].childCount++;
			return;
		}
	}
}

void resolveNode(nodeIndex nodePos) {
	//skip defNodes
	if (
		nodes[nodePos].evaluate == eval_varDef ||
		nodes[nodePos].evaluate == eval_fnDef  ||
		nodes[nodePos].evaluate == eval_stateDef
	) {
		return;
	}
	
	char *nodeName = nodesInfo[nodePos].name;
	int   nodeLine = nodesInfo[nodePos].line;
	
	//check for number literal
	if ( isNumeric(nodeName[0]) ) {
		for (int namePos = 0; namePos != '\0'; namePos++) {
			if (!( isNumeric(nodeName[namePos]) )) {
				putError(nodeLine, "invalid number literal '");
				printf("%s'\n", nodeName);
				free(nodeName);
				return;
			}
		}
		
		nodes[nodePos].evaluate = eval_numLit;
		sscanf(nodeName, "%lf", &nodes[nodePos].output.n);
		nodesInfo[nodePos].name = "numLit num";
		free(nodeName);
		return;
	}
	
	
	//check for output
	
	//check for reference to stdNode
	for (int sntPos = 0; sntPos < stdNodeTableLength; sntPos++) {
		if (matchStrXDelim(
			nodeName, '\0', stdNodeTable[sntPos]->name, ' '
		)) {
			if (nodes[nodePos].childCount != stdNodeTable[sntPos]->childCount) {
				putError(nodeLine, "");
				printf(
					"number of arguments for '%s' is off by %d\n",
					nodeName,
					nodes[nodePos].childCount - stdNodeTable[sntPos]->childCount
				);
				free(nodeName);
				return;
			}
			nodes[nodePos].evaluate = stdNodeTable[sntPos]->evaluate;
			nodesInfo[nodePos].name = stdNodeTable[sntPos]->name;
			free(nodeName);
			return;
		}
	}
	
	//check for frameform index
	//check for local stateCall
	//check for nonlocal stateCall
	//check for fnCall or varCall
	for (int rnPos = 0; rnPos <= currentRootNode; rnPos++) {
		if (matchStrXDelim(
			nodeName, '\0', nodesInfo[ rootNodes[rnPos] ].name, ' '
		)) {
			nodesInfo[nodePos].name = nodesInfo[ rootNodes[rnPos] ].name;
			//if it's a function
			if (nodes[ rootNodes[rnPos] ].evaluate == eval_fnDef) {
				nodes[nodePos].definition = rootNodes[rnPos];
				nodes[nodePos].evaluate = eval_fnCall;
				if (
					nodes[nodePos].childCount != nodesInfo[ rootNodes[rnPos] ].paramCount
				) {
					putError(nodeLine, "");
					printf(
						"number of arguments for '%s' is off by %d\n",
						nodeName,
						nodes[nodePos].childCount
							- nodesInfo[ rootNodes[rnPos] ].paramCount
					);
					free(nodeName);
					return;
				}
			}
			//else it's a variable
			else {
				nodes[currentNode].definition = rootNodes[rnPos];
				nodes[currentNode].evaluate = eval_varCall;
				//if (nodes[nodePos].childCount != 0)
					//calling a function held by a variable?
			}
			free(nodeName);
			return;
		}
	}
	//check for argCall
	for (int backNode = nodePos;; backNode++) {
		if (nodesInfo[backNode].level == 0) {
			if (
				nodes[backNode].evaluate == eval_fnDef &&
				nodesInfo[backNode].paramCount
			) {
				int   namePos  = 1;
				int   paramPos = 0;
				char *backNodeName = nodesInfo[backNode].name;
				for (; backNodeName[namePos] != '\0'; paramPos++) {
					for (;
						backNodeName[namePos-1] != '\n' &&
						backNodeName[namePos  ] != '\0'; 
						namePos++
					);//bodiless!
					if (matchStrXDelim(
						nodeName, '\0', &backNodeName[namePos], ' '
					)) {
						nodes[nodePos].definition  = backNode;
						nodes[nodePos].argRefIndex = paramPos;
						nodes[nodePos].evaluate    = eval_argCall;
						nodesInfo[nodePos].name = &backNodeName[namePos];
						free(nodeName);
						return;
					}
				}
				break;
			}
			else break;
		}
	}
	
	//none of the above
	putError(nodeLine, "did not recognize ");
	printf("'%s'\n", nodeName);
}

void initFrameform(void) {
	//check for proper frameform name
	if (lineBuf[1] == '\0') {
		putError(currentLine, "This frameform must have a name\n");
		return;
	}
	else if ( isNumeric( lineBuf[1] ) ) {
		putError(
			currentLine,
			"frameform names may not begin with numeric characters\n"
		);
		return;
	}
	//allow no spaces in the frameform name
	if ( strchr(&lineBuf[1], ' ') ) {
		putError(currentLine, "frameform names may not contain spaces\n");
		return;
	}
	
	//check for collisions, then inc_currentFrameform
	//inFrameform = false;
	//if ( findNameCollision(&lineBuf[1], '\0') )
	//	return;
	inFrameform = true;
	inc_currentFrameform();
	
	//copy from lineBuf to frameforms[currentFrameform].name
	strncpy(
		&frameforms[currentFrameform].name[0],
		&lineBuf[1],
		maxTokenLength
	);
}

void  parse(void) {
	//go through the file(s) line by line
	while (!errorCount) {
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
				initFrameform();
		}
		//we must have a defNode
		else
			initNodes();
	}
	
	//connect each node
	for (int nodePos = 0; nodePos <= currentNode; nodePos++)
		connectNode(nodePos);
	//resolve each node
	for (int nodePos = 0; nodePos <= currentNode; nodePos++)
		resolveNode(nodePos);
	if (errorCount)
		return;
	
	//check for type errors
}
