/*
 * counter.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#include "counter.h"
#include <stdio.h>
#include <unistd.h>

Counter::Counter(unsigned short int w, unsigned short int v)
{
	bool explore = true;
	listen_time.tv_sec = w;
	listen_time.tv_usec = 0;
	timeout_time.tv_sec = v;
	timeout_time.tv_usec = 0;
	gettimeofday(&last_time, NULL);
}

COUNTER_STATE Counter::check()
{
	struct timeval curr_time;
	struct timeval time_elapsed;

	gettimeofday(&curr_time, NULL);
	timersub(&curr_time, &last_time, &time_elapsed);

	if (explore)
	{
		// wait (time_to_wait - time_elapsed_millisec)
		if (timercmp(&listen_time, &time_elapsed, >))
			return LISTEN;

		explore = false;
		gettimeofday(&last_time, NULL);
		return PING;
	}
	else
	{
		// wait (time_to_wait - time_elapsed_millisec)
		if (timercmp(&timeout_time, &time_elapsed, >))
			return LISTEN;

		explore = true;
		gettimeofday(&last_time, NULL);
		return EXPLORE;
	}
}
