
#ifdef __linux__

#include <time.h>

long  getMicroseconds() {
	struct timespec  ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return  ts.tv_sec * 1e6  +  ts.tv_nsec / 1000;
}

#define microSleep usleep


#elif __APPLE__
#elif _WIN32
#endif
