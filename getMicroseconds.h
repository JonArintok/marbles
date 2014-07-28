


//#define _POSIX_C_SOURCE >= 199309L
#include <time.h>
long  getMicroseconds() {
	struct timespec  ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return  ts.tv_sec * 1e6  +  ts.tv_nsec / 1000;
}

