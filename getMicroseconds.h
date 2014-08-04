
#include <time.h>

long  getMicroseconds() {
#ifdef __linux__
	struct timespec  ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return  ts.tv_sec * 1e6  +  ts.tv_nsec / 1000;
#elif __APPLE__
#elif _WIN32
#endif
}
