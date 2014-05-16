
#include <stdint.h>

typedef struct {
	double    *data;
	uint32_t   dimensionality;
	uint32_t  *dimensions;
} array;
struct node;
typedef struct {
	struct node *data;
	uint32_t     dimensionality;
	uint32_t    *dimensions;
} nodeArray;
typedef union {
	double       n;//number
	struct node *f;//function
	array        N;//number array
	nodeArray    F;//function array
} UOE;         //union of everything
typedef void (*evaluator)(struct node*);
typedef struct node {
	char        *name;
	char        *typeString;
	char         evaluated;
	uint8_t      arity;
	struct node *arguments;
	evaluator    evaluate;
	UOE          output;
} node;
