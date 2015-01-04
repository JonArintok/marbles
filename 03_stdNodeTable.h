


#define _biop_(eval_name, node_name, title, op, type) \
outType eval_name(_evalargs_) {\
	nodeIndex arg0 = nodes[self].children[0];\
	nodeIndex arg1 = nodes[self].children[1];\
	outType a = output(arg0, fnCallSource, fnCallArgs);\
	outType b = output(arg1, fnCallSource, fnCallArgs);\
	outType toBeReturned;\
	toBeReturned.type = a.type op b.type;\
	return toBeReturned;\
}\
const stdNode node_name = {\
	.name = title " N1\na N1\nb N1",\
	.arity    = 2,\
	.evaluate = eval_name\
};



#include "stdNode/math.h"
#include "stdNode/logic.h"
#include "stdNode/runState.h"
#include "stdNode/tuples.h"
#include "stdNode/convert.h"
#include "stdNode/arrays.h"

const stdNode *stdNodeTable[] = {

//math
&node_add,
&node_sub,
&node_mul,
&node_div,
&node_mod,
&node_sqr,
&node_add4,
&node_add8,
&node_sin,

//logic
&node_if,
&node_equal,
&node_lessThan,
&node_greaterThan,
&node_notEqual,
&node_greaterThanOrEqual,
&node_lessThanOrEqual,
&node_both,
&node_either,
&node_not,

//runState
&node_curFrame,
&node_frameRate,
&node_videoWidth,
&node_videoHeight,
&node_windowWidth,
&node_windowHeight,

//tuples
&node_N4,
&node_fillB4,

//convert
&node_B1from01,
&node_B4from01,

//arrays
&node_buildB4D2,
&node_buildN1D2,
&node_fillB4D2,
&node_mapB4D2,
&node_mapN1D2,
&node_mapInB4D2,
&node_mapInN1D2,
&node_randomBoolFillN1D2,
&node_fromN1D2,
&node_widthOf,
&node_heightOf,

NULL
};

