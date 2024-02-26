#include "timekeeper.h"
#include <stdio.h>
#include <time.h>

/* Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both
 * windows and linux. */
long long get_unix_time_ms() {
	struct timeval tv;
#if defined(__MINGW32__) || defined(__MINGW64__)
	mingw_gettimeofday(&tv, NULL);
#else
	gettimeofday(&tv, NULL);
#endif

	long long unix_time_ms = (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
	//printf("Unix milliseconds: \t%llu\n", unix_time_ms);
	return unix_time_ms;
}

bool isPastTime(long long inputMilliseconds) {
	return inputMilliseconds < get_unix_time_ms();
}