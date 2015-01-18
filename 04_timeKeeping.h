
#ifdef __linux__

long  getMicroseconds() {
	struct timespec  ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return  ts.tv_sec * 1e6  +  ts.tv_nsec / 1000;
}


#elif __APPLE__
#elif _WIN32
#endif



void frameWait_default(long *prevTime) {
	if (frameRate > 0) {
		long timeWait = 1e6/frameRate - (getMicroseconds() - *prevTime);
		if (timeWait > 0.01) 
			microSleep(timeWait);
	}
	//printf("%li\n", getMicroseconds() - *prevTime);
	*prevTime = getMicroseconds();
}

void (*frameWait)(long *prevTime) = frameWait_default;

