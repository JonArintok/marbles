


#define _biop_(eval_name, node_name, title, op, type) \
outType eval_name(_evalargs_) {\
	nodeIndex arg0 = nodes[self].children[0];\
	nodeIndex arg1 = nodes[self].children[1];\
	outType a = _output_(arg0, fnCallArgs)\
	outType b = _output_(arg1, fnCallArgs)\
	outType toBeReturned;\
	toBeReturned.type = a.type op b.type;\
	return toBeReturned;\
}\
const stdNode node_name = {\
	.name = title " num\na num\nb num",\
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
&node_sqr,
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
&node_memcmp,

//runState
&node_curFrame,
&node_frameRate,
&node_windowWidth,
&node_windowHeight,

//tuples
&node_num4,
&node_fillByte4,

//convert
&node_byteFrom01,
&node_byte4From01,

//arrays
&node_buildByte4D2,
&node_buildNumD2,
&node_fillByte4D2,
&node_dynamicFillByte4D2,
&node_dynamicFillNumD2,
&node_limitedDynamicFillByte4D2,
&node_limitedDynamicFillNumD2,
&node_randomBoolFillNumD2,
&node_fromNumD2,
&node_widthOf,
&node_heightOf,

NULL
};

