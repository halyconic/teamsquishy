/*
 * counter.h
 *
 *  Created on: Oct 23, 2012
 *      Author: swebber
 */

#ifndef COUNTER_H_
#define COUNTER_H_

#include <sys/time.h>

class Counter
{
public:
	Counter(double rate);

	// Wait until last_time + wait_time
	void wait();

	// Return true and reset last_time if timeout has expired
	bool check();

	// reconstruct counter
	void reset(unsigned int microseconds);

	struct timeval last_time;
	struct timeval wait_time;
};

#endif /* COUNTER_H_ */
