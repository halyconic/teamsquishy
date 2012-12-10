/*
 * counter.h
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#ifndef COUNTER_H_
#define COUNTER_H_

#include <sys/time.h>

class Counter
{
public:
	// Time to wait in seconds
	Counter(unsigned short int wait_time = 4);

	// Return true and reset last_time if timeout has expired
	bool check();

	struct timeval last_time;
	struct timeval wait_time;
};

#endif /* COUNTER_H_ */
