//this is a temporary test to help me figure out how to implement 
//user-defined variables and functions without having to worry 
//about parsing


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "foundation.h"
#include "stdNodeTable.h"




int main(void) {
	
	
	//num  foo
	nodes[0] = node_num_varDef;
	nodes[0].name == "num  foo";
	
	//	34
	nodes[1] = node_numLit;
	nodes[1].output.n = 34;
	
	
	
	//fn  add 2 mul 2
	nodes[2] = {
		.name          = "fn  add 2 mul 2",
		.inTypeString  = "num",
		.argString     = "in",
		.outTypeString = "num",
		.arity         = 1,
		.arguments     = {3},
		.evaluate      = eval_fnDef,
		.output.n      = 0
	};
	
	//	mul
	nodes[3] = node_mul;
	nodes[3].arguments[0] = 4;// = {4,5};
	nodes[3].arguments[1] = 5;
	
	//		2
	nodes[4] = node_numLit;
	nodes[4].output.n = 2;
	
	//		add
	nodes[5] = node_add;
	nodes[5].arguments[0] = 6;
	nodes[5].arguments[1] = 7;
	
	//			2
	nodes[6] = node_numLit;
	nodes[6].output.n = 2;
	
	//			in
	nodes[7] = 
	
	
	
	
	
	return 0;
}



/*


	//fn  do to it
	nodes[???] = {
		.name          = "do to it",
		.inTypeString  = "fn\n\tnum\n\tnum\nnum",
		.argString     = "dot this\nwith this",
		.outTypeString = "num",
		.arity         = 0,
		.arguments     = {},
		.evaluate      = eval_fnCall,
		.output.n      = 0
	};


*/

