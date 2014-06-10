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
	nodes[0].name = "num  foo  34";	
	nodes[0].evaluate = eval_varDef;
	nodes[0].output.n = 34;
	
	
	
	//fn  double
	nodes[1] = {
		.name = 
			"fn  double\n"
			"\tnum  in\n"
			"\tis  num",
		.arity     = 1,
		.arguments = {},
		.evaluate  = eval_fnDef,
		.output.n  = 0
	};
	
	//	mul
	nodes[2] = node_mul;
	nodes[2].arguments[0] = 3;
	nodes[2].arguments[1] = 4;
	
	//		2
	nodes[3] = node_numLit;
	nodes[3].output.n = 2;
	
	//		in
	nodes[4] = {
		.name = 
			"fn  double\n"
			"\tnum  in\n"
			"\tis  num",
		.argRefIndex = 0,
		.arity       = 0,
		.evaluate    = eval_argCall,
	};
	
	
	
	//num  out A
	nodes[5] = {
		.name      = "num  out A",
		.arity     = 1,
		.arguments = {6},
		.evaluate  = eval_state
	};
	
	//	double
	nodes[6] = {
		.name      = "double",
		.fnDef     = 1,         //the nodeIndex of "fn  double"
		.arity     = 1,
		.arguments = {7},
		.evaluate  = eval_fnCall
	};
	
	//		foo
	nodes[7] = {
		.name      = "foo",
		.arity     = 0,
		.evaluate  = eval_varCall
	};
	
	
	
	return 0;
}


