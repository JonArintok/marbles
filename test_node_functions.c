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
	
	
	
	nodes[0] = {
		.name          = "foo",
		.inTypeString  = "",
		.outTypeString = "num",
		.arity         = 0,
		.arguments     = {},
		.evaluate      = eval_varCall,
		.output.n      = 34
	};
	nodes[???] = {
		.name          = "add 2 mul 2",
		.inTypeString  = "num",
		.argString     = "i",
		.outTypeString = "num",
		.arity         = 1,
		.arguments     = {},
		.evaluate      = eval_fnCall,
		.output.n      = 0
	};
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
	
	
	
	

	frameHead frameHeads[0] = {
		.name       = "main",
		.beginning  = ???,
		.length     = 2,
		.tasks      = ,
		.outputs    = 
	};
	
	
	nodes[???] = {
		.name          = "out A",
		.inTypeString  = "num",
		.outTypeString = "num",
		.arity         = 1,
		.arguments     = {},
		.evaluate      = eval_State,
		.output.n      = 0
	};
	nodes[???] = {
		.name          = "out B",
		.inTypeString  = "num",
		.outTypeString = "num",
		.arity         = 1,
		.arguments     = {},
		.evaluate      = eval_State,
		.output.n      = 0
	};
	
	
	
	return 0;
}

