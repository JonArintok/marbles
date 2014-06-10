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
	nodes[0].name == "num  foo  34";	
	nodes[0].output.n = 34;
	
	
	
	//fn  double
	nodes[1] = {
		.name = 
			"fn  double\n"
			"\tnum  in\n"
			"\tis  num",
		.arity         = 1,
		.arguments     = {},
		.evaluate      = eval_fnDef,
		.output.n      = 0
	};
	
	//	mul
	nodes[2] = node_mul;
	nodes[2].arguments[0] = 3;// = {4,5};
	nodes[2].arguments[1] = 4;
	
	//		2
	nodes[3] = node_numLit;
	nodes[3].output.n = 2;
	
	//		in
	nodes[4]
	
	
	
	//num  out A
	nodes[5] = 
	
	//	double
	nodes[6] = 
	
	//		foo
	nodes[7] = 
	
	
	
	return 0;
}


