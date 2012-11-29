/*
 * counter.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: swebber
 */

#include "counter.h"
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <sys/time.h>
#include <algorithm>

Counter::Counter(double rate)
{
	// stores the time of day in last_time
	// can access individual seconds and milliseconds by doing last_time->tv_sec or tv_usec
	gettimeofday(&last_time, NULL);

	wait_time.tv_usec = 1000000 * 1/rate;
}

Counter::Counter(unsigned long int timeout)
{
	wait_time.tv_sec = 0;
	wait_time.tv_usec = 1000 * timeout;
	gettimeofday(&last_time, NULL);
}

void Counter::wait()
{
	struct timeval curr_time;
	struct timeval time_elapsed;

	gettimeofday(&curr_time, NULL);
	timersub(&curr_time, &last_time, &time_elapsed);

	// wait (time_to_wait - time_elapsed_millisec)
	if (timercmp (&wait_time, &time_elapsed, >))
	{
		timersub(&wait_time, &time_elapsed, &time_elapsed);
		usleep(time_elapsed.tv_usec);
	}

	gettimeofday(&last_time, NULL);
}

void Counter::reset(unsigned int timeout)
{
	wait_time.tv_usec = 1000 * timeout;
	gettimeofday(&last_time, NULL);
}

// Return true if ready;
bool Counter::check()
{
	struct timeval curr_time;
	struct timeval time_elapsed;

	gettimeofday(&curr_time, NULL);
	timersub(&curr_time, &last_time, &time_elapsed);

	// wait (time_to_wait - time_elapsed_millisec)
	if (timercmp (&wait_time, &time_elapsed, >))
		return false;

	gettimeofday(&last_time, NULL);
	return true;
}
