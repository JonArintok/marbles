

#define charTag_comment   '|'
#define charTag_frameform '@'
#define charTag_paramType '&'
#define charTag_shareRead ':'
#define decTag_var   "var"
#define decTag_fn    "fn"
#define decTag_state "state"
#define decTag_share "share"
#define entryPointName "enter"
#define exitPointName  "exit"

FILE *fileStream;
char *fileName;
char  lineBuf[maxLineLength];

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
			lineBuf[0] = '\0';
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
			//read through to the newline character
			while ( (fileChar = fgetc(fileStream)) != '\n' )
				if (fileChar == EOF) 
					reachedEOF = true;
		}
		
		//check for newline character
		if (fileChar == '\n') {
			//end of the line
			while (bufPos && lineBuf[bufPos-1] == ' ')
				bufPos--;
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
			int wpos = 0;
			for (; s[wpos]; wpos++)//"uninitialised value" from valgrind sometimes?
				s[wpos] = s[++rpos];
			return wpos;
		}
		else if (!s[rpos])
			return 0;
	}
}

void printUpTo(char *s, char d) {
	for (int i = 0; s[i] != d; putc(s[i++], stdout));
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

void getBody(void) {
	while (true) {
		int level  = 0;
		int peren  = 0;
		int bufPos = 0;
		char prevDelim = '\n';
		fpos_t filePos;
		fgetpos(fileStream, &filePos);
		getLine();
		for (; lineBuf[bufPos] == '\t'; bufPos++)
			level++;
		if (!level) {
			if (lineBuf[0]) {
				fsetpos(fileStream, &filePos);
				curLine--;
			}
			break;
		}
		inc_curNode();
		for (; prevDelim; bufPos++) {
			switch (lineBuf[bufPos]) {
				case ' ':
					setBodyNode(level);
					inc_curNode();
					if (prevDelim == '(' || prevDelim == '\n')
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

void initNodes(void) {
	//"defNodes" are variables, functions, and stateNodes
	inc_curNode();
	nodes[curNode].childCount = 1;
	nodes[curNode].children[0] = curNode + 1;
	nodesInfo[curNode].line = curLine;
	if (inFrameform)
		nodesInfo[curNode].frameform = curFrameform;
	
	
	//read the first line into curNode.name
	inc_namePos();
	for (; lineBuf[namePos] != '\0'; inc_namePos())
		nodesInfo[curNode].name[namePos] = lineBuf[namePos];
	
	
	//determine what is being declared based on the decTag
	if (matchStrWDelim(decTag_fn, '\0', lineBuf, ' ')) {
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
		nodes[curNode].evaluate = paramCount ? eval_fnDef : eval_fnDefN;
		//else replace that \0 with a \n and read the parameters into the name
		for (int i = 0; i < paramCount; i++) {
			nodesInfo[curNode].name[namePos] = '\n';
			getLine();
			int bufPos = 0;//would be -1 if we weren't ignoring the tabs
			while (lineBuf[bufPos]) {
				inc_namePos();
				bufPos++;
				nodesInfo[curNode].name[namePos] = lineBuf[bufPos];
			}
		}
	}
	else if (matchStrWDelim(decTag_var, '\0', lineBuf, ' '))
		nodes[curNode].evaluate = eval_varDef;
	else if (matchStrWDelim(decTag_state, '\0', lineBuf, ' '))
		nodes[curNode].evaluate = eval_stateDef;
	else if (matchStrWDelim(decTag_share, '\0', lineBuf, ' '))
		nodes[curNode].evaluate = eval_shareDef;
	else {
		putError(curLine, "decTag is not recognized\n");
		return;
	}
	
	//the decTag is no longer needed
	strRemoveUpToIncl(nodesInfo[curNode].name, ' ');
	
	//update the relevant node reference array
	if (
		nodes[curNode].evaluate == eval_stateDef ||
		nodes[curNode].evaluate == eval_shareDef
	) {
		if (!inFrameform) {
			putError(curLine, "state declared outside of frameform\n");
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
	getBody();
}

void initOutput(void) {
	//check for global output declaration
	if (matchStrWDelim(frameRateName, ' ', lineBuf, '\0'))
		frameRateRoot = curNode+1;
	else if (matchStrWDelim(windowWidthName, ' ', lineBuf, '\0')) {
		windowWidthRoot = curNode+1;
		videoEnabled = true;
	}
	else if (matchStrWDelim(windowHeightName, ' ', lineBuf, '\0')) {
		windowHeightRoot = curNode+1;
		videoEnabled = true;
	}
	//check for frameform output declaration
	else if (matchStrWDelim(nextFrameformName, ' ', lineBuf, '\0')) {
		if (!inFrameform) {
			putError(curLine, "cannot declare '");
			printf("%s' outside of frameform\n", nextFrameformName);
			return;
		}
		frameforms[curFrameform].nextFrameform = curNode+1;
	}
	else if (matchStrWDelim(videoOutName, ' ', lineBuf, '\0')) {
		if (!inFrameform) {
			putError(curLine, "cannot declare '");
			printf("%s' outside of frameform\n", videoOutName);
			return;
		}
		frameforms[curFrameform].videoOut = curNode+1;
		videoEnabled = true;
	}
	else if (matchStrWDelim(audioOutName, ' ', lineBuf, '\0')) {
		if (!inFrameform) {
			putError(curLine, "cannot declare '");
			printf("%s' outside of frameform\n", audioOutName);
			return;
		}
		frameforms[curFrameform].audioOut = curNode+1;
		audioEnabled = true;
	}
	getBody();
}

void attachFnOrVarCall(nodeIndex def, nodeIndex call) {
	nodesInfo[call].name = nodesInfo[def].name;
	nodes[call].definition = def;
	//if it's a function with parameters
	if (nodes[def].evaluate == eval_fnDef) {
		if (!nodes[call].childCount) {
			nodes[call].evaluate = eval_fnPass;
			nodes[call].cache.f = def;
		}
		else if (nodes[call].childCount == nodesInfo[def].arity) {
			nodes[call].evaluate = eval_fnCall;
		}
		else {
			putError(nodesInfo[call].line, "");
			printf(
				"number of arguments for '%s' is off by %d\n",
				nodesInfo[call].name,
				nodes[call].childCount - nodesInfo[def].arity
			);
			return;
		}
	}
	//else perhaps it's a nullary function
	else if (nodes[def].evaluate == eval_fnDefN)
		nodes[call].evaluate = eval_fnCallN;
	//else it's a variable
	else if (nodes[def].evaluate == eval_varDef) {
		nodes[call].evaluate = eval_varCall;
		//if (nodes[call].childCount != 0)
		//	calling a function held by a variable?
	}
	else _shouldNotBeHere_
}

void resolveNode(nodeIndex nodePos) {
	//skip defNodes
	if (!nodesInfo[nodePos].level)
		return;
	
	char *nodeName = nodesInfo[nodePos].name;
	int   nodeLine = nodesInfo[nodePos].line;
	int   nodeFf   = nodesInfo[nodePos].frameform;
	
	//check for number literal
	if (isdigit(nodeName[0]) || (nodeName[0] == '-' && isdigit(nodeName[1]))) {
		for (int namePos = 0; namePos; namePos++) {
			if (!( isNumeric(nodeName[namePos]) )) {
				putError(nodeLine, "invalid number literal '");
				printf("%s'\n", nodeName);
				free(nodeName);
				return;
			}
		}
		nodes[nodePos].evaluate = eval_numLit;
		sscanf(nodeName, "%lf", &nodes[nodePos].cache.n);
		nodesInfo[nodePos].name = name_numLit;
		free(nodeName);
		return;
	}
	
	//check for argCall
	for (int backNode = nodePos;; backNode--) {
		if (nodesInfo[backNode].level == 0) {
			if (nodes[backNode].evaluate == eval_fnDef) {
				int   bnNamePos  = 1;
				int   paramPos   = 0;
				char *bnNodeName = nodesInfo[backNode].name;
				for (; 
					bnNodeName[bnNamePos] && paramPos < nodesInfo[backNode].arity;
					paramPos++
				) {
					while (bnNodeName[bnNamePos] && bnNodeName[bnNamePos-1] != '\n')
						bnNamePos++;
					if (matchStrWDelim(nodeName, '\0', &bnNodeName[bnNamePos], ' ')) {
						nodes[nodePos].argRefIndex = paramPos;
						nodesInfo[nodePos].name    = &bnNodeName[bnNamePos];
						free(nodeName);
						nodes[nodePos].evaluate = eval_argCall;
						for (
							int paramNamePos = 0;
							nodesInfo[nodePos].name[paramNamePos] &&
							nodesInfo[nodePos].name[paramNamePos] != '\n';
							paramNamePos++
						) {
							if (
								nodesInfo[nodePos].name[paramNamePos] == charTag_paramType &&
								nodes[nodePos].childCount
							) {
								nodes[nodePos].evaluate = eval_fnArgCall;
								break;
							}
						}
						return;
					}
					bnNamePos++;
				}
				break;
			}
			else break;
		}
	}
	
	//check for reference to std fn
	for (int sntPos = 0; sntPos < stdNodeTableLength; sntPos++) {
		if (matchStrWDelim(nodeName, '\0', stdNodeTable[sntPos]->name, ' ')) {
			if (!nodes[nodePos].childCount && stdNodeTable[sntPos]->arity) {
				nodes[nodePos].evaluate = eval_fnPass;
				nodes[nodePos].cache.f = sntPos + curNode + 1;
			}
			else if (nodes[nodePos].childCount == stdNodeTable[sntPos]->arity) {
				nodes[nodePos].evaluate = stdNodeTable[sntPos]->evaluate;
			}
			else {
				putError(nodeLine, "");
				printf(
					"number of arguments for '%s' is off by %d\n",
					nodeName,
					nodes[nodePos].childCount - stdNodeTable[sntPos]->arity
				);
				free(nodeName);
				return;
			}
			nodesInfo[nodePos].name = stdNodeTable[sntPos]->name;
			free(nodeName);
			return;
		}
	}
	
	//check for frameform index
	for (int ffPos = 0; ffPos <= curFrameform; ffPos++) {
		if (!(strcmp(nodeName, frameforms[ffPos].name))) {
			nodes[nodePos].cache.n = ffPos;
			nodes[nodePos].evaluate = eval_frameformRef;
			nodesInfo[nodePos].name = name_frameformRef;
			free(nodeName);
			return;
		}
	}
	//check for reference to exit
	if (!(strcmp(nodeName, exitPointName))) {
		nodes[nodePos].cache.n = exitFrameform;
		nodes[nodePos].evaluate = eval_frameformRef;
		nodesInfo[nodePos].name = name_frameformRef;
		free(nodeName);
		return;
	}
	
	//check for reference to local declaration
	if (nodeFf > -1) {
		//check for local state or share call
		for (int snPos = 0; snPos <= frameforms[nodeFf].curStateNode; snPos++) {
			nodeIndex sDef = frameforms[nodeFf].stateNodes[snPos];
			if (matchStrWDelim(nodeName, '\0', nodesInfo[sDef].name, ' ')) {
				nodes[nodePos].definition = sDef;
				nodesInfo[nodePos].name = nodesInfo[sDef].name;
				free(nodeName);
				if (nodes[sDef].evaluate == eval_stateDef)
					nodes[nodePos].evaluate = eval_stateCall;
				else if (nodes[sDef].evaluate == eval_shareDef)
					nodes[nodePos].evaluate = eval_shareCall;
				else _shouldNotBeHere_
				return;
			}
		}
		//check for local fnCall or varCall
		for (int rnPos = 0; rnPos <= frameforms[nodeFf].curRootNode; rnPos++) {
			if (matchStrWDelim(
				nodeName, '\0',
				nodesInfo[ frameforms[nodeFf].rootNodes[rnPos] ].name, ' '
			)) {
				attachFnOrVarCall(frameforms[nodeFf].rootNodes[rnPos], nodePos);
				free(nodeName);
				return;
			}
		}
	}
	
	//check for nonlocal share Call
	for (int nnPos = 0; nodeName[nnPos]; nnPos++) {
		if (nodeName[nnPos] == charTag_shareRead){
			if (!nnPos) {
				putError(nodeLine, "expected the name of a frameform before '");
				printf("%c'\n", charTag_shareRead);
				free(nodeName);
				return;
			}
			if (!nodeName[nnPos+1]) {
				putError(
					nodeLine, 
					"expected the name of some state declared with '"
				);
				printf("%s' afer '%c'\n", decTag_share, charTag_shareRead);
				free(nodeName);
				return;
			}
			for (int ffPos = 0; ffPos <= curFrameform; ffPos++) {
				if (ffPos == nodeFf)
					continue;
				if (matchStrWDelim(
					nodeName, charTag_shareRead, frameforms[ffPos].name, '\0'
				)) {
					for (
						int snPos = 0;
						snPos <= frameforms[ffPos].curStateNode;
						snPos++
					) {
						nodeIndex sDef = frameforms[ffPos].stateNodes[snPos];
						if (matchStrWDelim(
							&nodeName[nnPos+1], '\0',
							nodesInfo[sDef].name, ' '
						)) {
							if (nodes[sDef].evaluate == eval_stateDef) {
								putError(nodeLine, "cannot read '");
								printf("%s' from '", &nodeName[nnPos+1]);
								printUpTo(nodeName, charTag_shareRead);
								puts("'");
								free(nodeName);
								return;
							}
							nodes[nodePos].definition = sDef;
							nodesInfo[nodePos].name = nodesInfo[sDef].name;
							free(nodeName);
							if (nodes[sDef].evaluate == eval_shareDef)
								nodes[nodePos].evaluate = eval_shareCall;
							else _shouldNotBeHere_
							return;
						}
					}
					putError(nodeLine, "did not find '");
					printf("%s' in '", &nodeName[nnPos+1]);
					printUpTo(nodeName, charTag_shareRead);
					puts("'");
					free(nodeName);
					return;
				}
			}
			if (matchStrWDelim(
				nodeName, charTag_shareRead, frameforms[nodeFf].name, '\0'
			)) {
				putError(nodeLine, "attempted to read local state '");
				printf("%s' as if it were nonlocal\n", &nodeName[nnPos+1]);
				free(nodeName);
				return;
			}
			putError(nodeLine, "did not recognize frameform '");
			printUpTo(nodeName, charTag_shareRead);
			puts("'");
			free(nodeName);
			return;
		}
	}
	
	//check for global fnCall or varCall
	for (int grnPos = 0; grnPos <= gCurRootNode; grnPos++) {
		if (matchStrWDelim(
			nodeName, '\0', nodesInfo[ gRootNodes[grnPos] ].name, ' '
		)) {
			attachFnOrVarCall(gRootNodes[grnPos], nodePos);
			free(nodeName);
			return;
		}
	}
	
	//none of the above
	putError(nodeLine, "did not recognize ");
	printf("'%s'\n", nodeName);
	free(nodeName);
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
	//allow no declarations of exitPointName
	if (!(strcmp(&lineBuf[1], exitPointName))) {
		putError(curLine, "cannot name a frameform '");
		printf("%s'\n", exitPointName);
		return;
	}
	
	inc_curFrameform();
	inFrameform = true;
	frameforms[curFrameform].line = curLine;
	
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
		//check for output, which should contain no spaces or tildes
		else if (!(strchr(&lineBuf[0], ' ')))
			initOutput();
		//we must have a defNode
		else
			initNodes();
	}
	
	//the exitFrameform is not actually a frameform, just a number
	exitFrameform = curFrameform + 1;
	
	//resolve each node
	for (int nodePos = 0; nodePos <= curNode; nodePos++)
		resolveNode(nodePos);
	if (errorCount)
		return;
	
	//check each frameform for errors
	for (int ffPos = 0; ffPos <= curFrameform; ffPos++) {
		if (frameforms[ffPos].nextFrameform < 0) {
			putError(frameforms[ffPos].line, "frameform '");
			printf("%s' needs a '", frameforms[ffPos].name);
			printUpTo(nextFrameformName, ' ');
			puts("' declaration");
			return;
		}
	}
	
}
