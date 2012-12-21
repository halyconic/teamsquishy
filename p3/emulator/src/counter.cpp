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
	time_elapsed.tv_usec = curr_time.tv_usec - last_time.tv_usec;

	if (explore)
	{
		// wait (time_to_wait - time_elapsed_millisec)
		if (listen_time.tv_usec > time_elapsed.tv_usec)
			return PING;

		explore = false;
		gettimeofday(&last_time, NULL);
		return LISTEN;
	}
	else
	{
		// wait (time_to_wait - time_elapsed_millisec)
		if (timeout_time.tv_usec > time_elapsed.tv_usec)
			return LISTEN;

		explore = true;
		gettimeofday(&last_time, NULL);
		return EXPLORE;
	}
}
