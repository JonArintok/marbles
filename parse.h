

#define charTag_comment   '/'
#define charTag_frameform '#'
#define charTag_paramType '<'
#define scopeTag_state "state"
#define scopeTag_share "share"
#define scopeTag_out   "out"
#define nullaryTag "nullary"

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
	curLine++;
	
	//read the next line from the file one character at a time
	for (int bufPos = 0;; bufPos++) {
		char fileChar;
		
		//check to be sure we are not overflowing the buffer
		if (bufPos == maxLineLength) {
			putError(curLine, "line is too long\n");
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
		//ignore spaces around charTag_paramType
		//ignore spaces after charTag_frameform
		if (fileChar == ' ') {
			if (!bufPos) {
				putError(curLine, "leading space\n");
				return;
			}
			if (
				lineBuf[bufPos-1] == ' ' ||
				lineBuf[bufPos-1] == '(' ||
				lineBuf[bufPos-1] == ')' ||
				lineBuf[bufPos-1] == charTag_frameform ||
				lineBuf[bufPos-1] == charTag_paramType
			) {
				bufPos--;
				continue;
			}
		}
		if (
			fileChar == '(' ||
			fileChar == ')' ||
			fileChar == charTag_paramType
		) {
			if (!bufPos) {
				putError(curLine, "leading character '");
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
			putError(curLine, "unexpected tab character\n");
			return;
		}
		
		lineBuf[bufPos] = fileChar;
		
		//ignore comments
		if (fileChar == charTag_comment) {
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


void initNodes(void) {
	//"defNodes" are variables, functions, and stateNodes
	inc_curNode();
	nodes[curNode].childCount = 1;
	nodes[curNode].children[0] = curNode + 1;
	nodesInfo[curNode].line = curLine;
	nodesInfo[curNode].level = 0;
	
	char *nodeName = nodesInfo[curNode].name;
	
	//read the first line into curNode.name
	while (true) {
		inc_namePos();
		//check for number literal
		if (
			lineBuf[namePos] == ' ' &&
			isNumeric( lineBuf[namePos+1] )
		) {
			int bufPos = namePos;
			nodeName[namePos] = '\0';
			nodes[curNode].evaluate = eval_varDef;
			if (inFrameform) {
				inc_curRootNode();
				frameforms[curFrameform].rootNodes[curRootNode] = curNode;
			}
			else {
				inc_gCurRootNode();
				gRootNodes[gCurRootNode] = curNode;
			}
			inc_curNode();
			nodesInfo[curNode].line = curLine;
			nodesInfo[curNode].level = 1;
			while (lineBuf[bufPos] != '\0') {
				inc_namePos();
				bufPos++;
				nodeName[namePos] = lineBuf[bufPos];
			}
			return;
		}
		nodeName[namePos] = lineBuf[namePos];
		if (lineBuf[namePos] == '\0')
			break;
	}
	
	
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
				putError(curLine, "incomplete definition\n");
				break;
			}
		}
	}
	nodesInfo[curNode].arity = paramCount;
	
	
	/*
	
	ending:                  meaning:
	scopeTag_state           stateDef, local
	scopeTag_share           shareDef, local
	scopeTag_out             outDef, global or local
	nullaryTag               fnDefN, global or local
	(none), paramCount > 0   fnDef, global or local
	(none), paramCount == 0  varDef, global or local
	
	scopeTags are removed from nodesInfo[i].name
	the nullaryTag is part of the type declaration
	
	*/
	
	
	
	
	
	
	
	
	/*
	//determine if it's a stateDef, varDef, or fnDef
	if ( !paramCount && strcmp(&lineBuf[namePos-7], "nullary") ) {
		if (inFrameform) {
			nodes[curNode].evaluate = eval_stateDef;
			inc_curStateNode();
			frameforms[curFrameform].stateNodes[
				frameforms[curFrameform].curStateNode
			] = curNode;
		}
		else
			nodes[curNode].evaluate = eval_varDef;
	}
	else {
		nodes[curNode].evaluate = eval_fnDef;
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
	//update the frameform or rootNode array
	if (nodes[curNode].evaluate == eval_stateDef) {
		inc_curStateNode();
		frameforms[curFrameform].stateNodes[
			frameforms[curFrameform].curStateNode
		] = curNode;
	}
	else {
		inc_gCurRootNode();
		gRootNodes[gCurRootNode] = curNode;
	}*/

	
	//initialize the nodes in the body of the defNode
	while (true) {
		int elevation = 0;
		int fold      = 0;
		int bufPos    = 0;
		char prevDelim = '\n';
		
		#define _setNodesInfo_\
			inc_namePos();\
			nodesInfo[curNode].name[namePos] = '\0';\
			nodesInfo[curNode].level = elevation + fold;\
			nodesInfo[curNode].line = curLine;\
			nodesInfo[curNode].frameform = inFrameform ? curFrameform : -1;
		
		getLine();
		for (; lineBuf[bufPos] == '\t'; bufPos++)
			elevation++;
		if (!elevation)
			break;
		inc_curNode();
		for (; prevDelim != '\0'; bufPos++) {
			switch (lineBuf[bufPos]) {
				case ' ':
					_setNodesInfo_
					inc_curNode();
					if (prevDelim != ' ')
						fold++;
					prevDelim = ' ';
					break;
				case '(':
					if (lineBuf[bufPos-1] != ')') {
						_setNodesInfo_
						inc_curNode();
					}
					if (prevDelim != ' ')
						fold++;
					prevDelim = '(';
					break;
				case ')':
					if (prevDelim == '(') {
						inc_namePos();
						nodesInfo[curNode].name[namePos] = '\0';
						putError(curLine, "superfluous perentheses around '");
						printf("%s'\n", nodesInfo[curNode].name);
						return;
					}
					if (lineBuf[bufPos-1] != ')') {
						_setNodesInfo_
					}
					if (lineBuf[bufPos+1] != '\0' && lineBuf[bufPos+1] != ')') {
						inc_curNode();
					}
					if (prevDelim == ' ')
						fold--;
					fold--;
					prevDelim = ')';
					break;
				case '\0':
					if (fold && prevDelim != ' ') {
						putError(curLine, "perentheses are off by ");
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
					nodesInfo[curNode].name[namePos] = lineBuf[bufPos];
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
	for (int rnPos = 0; rnPos <= gCurRootNode; rnPos++) {
		if (matchStrXDelim(
			nodeName, '\0', nodesInfo[ gRootNodes[rnPos] ].name, ' '
		)) {
			nodesInfo[nodePos].name = nodesInfo[ gRootNodes[rnPos] ].name;
			//if it's a function
			if (nodes[ gRootNodes[rnPos] ].evaluate == eval_fnDef) {
				nodes[nodePos].definition = gRootNodes[rnPos];
				nodes[nodePos].evaluate = eval_fnCall;
				if (
					nodes[nodePos].childCount != nodesInfo[ gRootNodes[rnPos] ].paramCount
				) {
					putError(nodeLine, "");
					printf(
						"number of arguments for '%s' is off by %d\n",
						nodeName,
						nodes[nodePos].childCount
							- nodesInfo[ gRootNodes[rnPos] ].paramCount
					);
					free(nodeName);
					return;
				}
			}
			//else it's a variable
			else {
				nodes[curNode].definition = gRootNodes[rnPos];
				nodes[curNode].evaluate = eval_varCall;
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
		putError(curLine, "This frameform must have a name\n");
		return;
	}
	else if ( isNumeric( lineBuf[1] ) ) {
		putError(
			curLine,
			"frameform names may not begin with numeric characters\n"
		);
		return;
	}
	//allow no spaces in the frameform name
	if ( strchr(&lineBuf[1], ' ') ) {
		putError(curLine, "frameform names may not contain spaces\n");
		return;
	}
	
	inc_curFrameform();
	inFrameform = true;
	
	//copy from lineBuf to frameforms[curFrameform].name
	strncpy(
		&frameforms[curFrameform].name[0],
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
		if (lineBuf[0] == charTag_frameform) {
			if (lineBuf[1] == charTag_frameform)
				inFrameform = false;
			else
				initFrameform();
		}
		//we must have a defNode
		else
			initNodes();
	}
	
	//check for naming collisions
	
	
	//connect each node
	for (int nodePos = 0; nodePos <= curNode; nodePos++)
		connectNode(nodePos);
	//resolve each node
	for (int nodePos = 0; nodePos <= curNode; nodePos++)
		resolveNode(nodePos);
	if (errorCount)
		return;
	
	//check for type errors
}
