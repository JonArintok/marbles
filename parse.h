

#define charTag_comment   '`'
#define charTag_frameform '@'
#define charTag_paramType '&'
#define decTag_var   "var"
#define decTag_fn    "fn"
#define decTag_state "state"
#define decTag_share "share"
#define entryPointName "enter"

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
	return  (in >=  '0' && in <=  '9') || in == '.';
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

bool matchStrWDelim(char *A, char AD, char *B, char BD) {
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

int strRemoveUpToIncl(char *s, char d) {
	for (int rpos = 0;; rpos++) {
		if (s[rpos] == d) {
			rpos++;
			int wpos = 0;
			for (;; wpos++) {
				s[wpos] = s[rpos+wpos];
				if (!s[wpos]) break;
			}
			return wpos;
		}
		else if (!s[rpos])
			return 0;
	}
}

void setBodyNode(int level) {
	inc_namePos();
	nodesInfo[curNode].name[namePos] = '\0';
	nodesInfo[curNode].level = level;
	nodesInfo[curNode].line = curLine;
	nodesInfo[curNode].frameform = inFrameform ? curFrameform : -1;
	
	//the nodes at levels 0 and 1 should already be connected
	if (level < 2)
		return;
	//look backwards for the node's parent
	for (nodeIndex backNode = curNode - 1;; backNode--) {
		if (nodesInfo[backNode].level == level - 1) {
			nodes[backNode].children[ nodes[backNode].childCount ] = curNode;
			nodes[backNode].childCount++;
			return;
		}
	}
}

void initNodes(void) {
	//"defNodes" are variables, functions, and stateNodes
	inc_curNode();
	nodes[curNode].childCount = 1;
	nodes[curNode].children[0] = curNode + 1;
	nodesInfo[curNode].line = curLine;
	
	
	//read the first line into curNode.name
	while (true) {
		inc_namePos();
		//check for in-line number literal
		if (
			lineBuf[namePos] == ' ' &&
			isNumeric( lineBuf[namePos+1] )
		) {
			int bufPos = namePos;
			nodesInfo[curNode].name[namePos] = '\0';
			nodes[curNode].evaluate = eval_varDef;
			if (inFrameform) {
				inc_curRootNode();
				frameforms[curFrameform].rootNodes[
					frameforms[curFrameform].curRootNode
				] = curNode;
			}
			else {
				inc_gCurRootNode();
				gRootNodes[gCurRootNode] = curNode;
			}
			inc_curNode();
			nodesInfo[curNode].line = curLine;
			nodesInfo[curNode].level = 1;
			while (lineBuf[bufPos]) {
				inc_namePos();
				bufPos++;
				nodesInfo[curNode].name[namePos] = lineBuf[bufPos];
			}
			return;
		}
		//else just read it into the node's name
		nodesInfo[curNode].name[namePos] = lineBuf[namePos];
		if (lineBuf[namePos] == '\0')
			break;
	}
	
	char *nodeName = nodesInfo[curNode].name;
	
	
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
	
	//determine what is being declared based on the decTag
	if (matchStrWDelim(decTag_fn, '\0', lineBuf, ' ')) {
		nodes[curNode].evaluate = paramCount ? eval_fnDef : eval_fnDefN;
		//else replace that \0 with a \n and read the parameters into the name
		for (int i = 0; i < paramCount; i++) {
			nodeName[namePos] = '\n';
			getLine();
			int bufPos = 0;//would be -1 if we weren't ignoring the tabs
			while (lineBuf[bufPos]) {
				inc_namePos();
				bufPos++;
				nodeName[namePos] = lineBuf[bufPos];
			}
		}
	}
	else if (matchStrWDelim(decTag_var, '\0', lineBuf, ' ')) {
		nodes[curNode].evaluate = eval_varDef;
		if (paramCount) {
			putError(curLine, "variable appears to be declared with arguments");
			return;
		}
	}
	else if (matchStrWDelim(decTag_state, '\0', lineBuf, ' '))
		nodes[curNode].evaluate = eval_stateDef;
	else if (matchStrWDelim(decTag_share, '\0', lineBuf, ' '))
		nodes[curNode].evaluate = eval_shareDef;
	else {
		putError(curLine, "decTag is not recognized");
		return;
	}
	
	//remove the decTag
	strRemoveUpToIncl(nodeName, ' ');
	
	//update the relevant node reference array
	if (
		nodes[curNode].evaluate == eval_stateDef ||
		nodes[curNode].evaluate == eval_shareDef
	) {
		if (!inFrameform) {
			putError(curLine, "state declared outside of frameform");
			return;
		}
		inc_curStateNode();
		frameforms[curFrameform].stateNodes[
			frameforms[curFrameform].curStateNode
		] = curNode;
	}
	else if (
		nodes[curNode].evaluate == eval_varDef ||
		nodes[curNode].evaluate == eval_fnDef  ||
		nodes[curNode].evaluate == eval_fnDefN
	) {
		if (inFrameform) {
			inc_curRootNode();
			frameforms[curFrameform].rootNodes[
				frameforms[curFrameform].curRootNode
			] = curNode;
		}
		else {
			inc_gCurRootNode();
			gRootNodes[gCurRootNode] = curNode;
		}
	}
	
	
	//initialize the nodes in the body of the defNode
	while (true) {
		int level  = 0;
		int peren  = 0;
		int bufPos = 0;
		char prevDelim = '\n';
		
		getLine();
		for (; lineBuf[bufPos] == '\t'; bufPos++)
			level++;
		if (!level)
			break;
		inc_curNode();
		for (; prevDelim; bufPos++) {
			switch (lineBuf[bufPos]) {
				case ' ':
					setBodyNode(level);
					inc_curNode();
					if (prevDelim != ' ')
						level++;
					prevDelim = ' ';
					break;
				case '(':
					if (lineBuf[bufPos-1] != ')') {
						setBodyNode(level);
						inc_curNode();
					}
					if (prevDelim != ' ' && prevDelim != ')')
						level++;
					prevDelim = '(';
					peren++;
					break;
				case ')':
					if (prevDelim == '(') {
						inc_namePos();
						nodesInfo[curNode].name[namePos] = '\0';
						putError(curLine, "superfluous perentheses around '");
						printf("%s'\n", nodesInfo[curNode].name);
						return;
					}
					if (lineBuf[bufPos-1] != ')')
						setBodyNode(level);
					if (lineBuf[bufPos+1] && lineBuf[bufPos+1] != ')')
						inc_curNode();
					level--;
					prevDelim = ')';
					peren--;
					break;
				case '\0':
					if (peren) {
						putError(curLine, "perentheses are off by ");
						printf("%d\n", peren);
						return;
					}
					else
						if (lineBuf[bufPos-1] != ')')
							setBodyNode(level);
					prevDelim = '\0';
					break;
				default:
					inc_namePos();
					nodesInfo[curNode].name[namePos] = lineBuf[bufPos];
			}
		}
	}
}

bool isDefNode(nodeIndex n) {
	if (
		nodes[n].evaluate == eval_varDef   ||
		nodes[n].evaluate == eval_fnDef    ||
		nodes[n].evaluate == eval_fnDefN   ||
		nodes[n].evaluate == eval_stateDef ||
		nodes[n].evaluate == eval_shareDef
	) {
		return true;
	}
	return false;
}

void resolveNode(nodeIndex nodePos) {
	//skip defNodes
	if (isDefNode(nodePos))
		return;
	
	char *nodeName = nodesInfo[nodePos].name;
	int   nodeLine = nodesInfo[nodePos].line;
	
	//check for number literal
	if (
		isdigit(nodeName[0]) ||
		( nodeName[0] == '-' && isdigit(nodeName[1]) )
	) {
		for (int namePos = 0; namePos; namePos++) {
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
	
	
	//check for reference to stdNode
	for (int sntPos = 0; sntPos < stdNodeTableLength; sntPos++) {
		if (matchStrWDelim(nodeName, '\0', stdNodeTable[sntPos]->name, ' ')) {
			if (nodes[nodePos].childCount != stdNodeTable[sntPos]->arity) {
				putError(nodeLine, "");
				printf(
					"number of arguments for '%s' is off by %d\n",
					nodeName,
					nodes[nodePos].childCount - stdNodeTable[sntPos]->arity
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
	//check for local state or share call
	//check for nonlocal share Call
	//check for local fnCall or varCall
	//check for global fnCall or varCall
	for (int rnPos = 0; rnPos <= gCurRootNode; rnPos++) {
		if (matchStrWDelim(
			nodeName, '\0', nodesInfo[ gRootNodes[rnPos] ].name, ' '
		)) {
			nodesInfo[nodePos].name = nodesInfo[ gRootNodes[rnPos] ].name;
			nodes[nodePos].definition = gRootNodes[rnPos];
			//if it's a function with parameters
			if (nodes[ gRootNodes[rnPos] ].evaluate == eval_fnDef) {
				nodes[nodePos].evaluate = eval_fnCall;
				if (
					nodes[nodePos].childCount != nodesInfo[ gRootNodes[rnPos] ].arity
				) {
					putError(nodeLine, "");
					printf(
						"number of arguments for '%s' is off by %d\n",
						nodeName,
						nodes[nodePos].childCount - nodesInfo[ gRootNodes[rnPos] ].arity
					);
					free(nodeName);
					return;
				}
			}
			//else perhaps it's a nullary function
			else if (nodes[ gRootNodes[rnPos] ].evaluate == eval_fnDef)
				nodes[nodePos].evaluate = eval_fnCallN;
			//else it's a variable
			else {
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
			if (nodes[backNode].evaluate == eval_fnDef) {
				int   namePos  = 1;
				int   paramPos = 0;
				char *backNodeName = nodesInfo[backNode].name;
				for (; backNodeName[namePos]; paramPos++) {
					for (;
						backNodeName[namePos-1] != '\n' && backNodeName[namePos];
						namePos++
					);//bodiless!
					if (matchStrWDelim(
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
	
	if (!(strcmp(&lineBuf[1], entryPointName)))
		activeFrameform = curFrameform;
	
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
			if (lineBuf[1] == '.')
				inFrameform = false;
			else
				initFrameform();
		}
		//we must have a defNode
		else
			initNodes();
	}
	
	//check for naming collisions
	
	
	//resolve each node
	for (int nodePos = 0; nodePos <= curNode; nodePos++)
		resolveNode(nodePos);
	if (errorCount)
		return;
	
	//check for type errors
}
