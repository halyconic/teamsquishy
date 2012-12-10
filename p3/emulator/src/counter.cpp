/*
 * counter.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#include "counter.h"
#include <stdio.h>
#include <unistd.h>

Counter::Counter(unsigned short int w)
{
	wait_time.tv_sec = w;
	wait_time.tv_usec = 0;
	gettimeofday(&last_time, NULL);
}

bool Counter::check()
{
	struct timeval curr_time;
	struct timeval time_elapsed;

	gettimeofday(&curr_time, NULL);
	time_elapsed.tv_usec = curr_time.tv_usec - last_time.tv_usec;

	// wait (time_to_wait - time_elapsed_millisec)
	if (wait_time.tv_usec > time_elapsed.tv_usec)
		return false;

	gettimeofday(&last_time, NULL);
	return true;
}
