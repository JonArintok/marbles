
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



void frameWait_default(long *prevTime) {
	if (frameRate < 0) {
		printf("frame time: %li\n", getMicroseconds() - *prevTime);
	}
	else {
		long timeWait = 1e6/frameRate - (getMicroseconds() - *prevTime);
		microSleep(timeWait);
	}
	*prevTime = getMicroseconds();
}

void (*frameWait)(long *prevTime) = frameWait_default;

