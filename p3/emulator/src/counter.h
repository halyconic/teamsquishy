/*
 * counter.h
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#ifndef COUNTER_H_
#define COUNTER_H_

#include <sys/time.h>

enum COUNTER_STATE
{
	PING,
	EXPLORE,
	LISTEN
};

class Counter
{
private:
	bool explore;

public:
	// Time to wait in seconds
	Counter(unsigned short int listen_time = 3, unsigned short int timeout_time = 1);

	// Return state and reset last_time if timeout has expired
	COUNTER_STATE check();

	struct timeval last_time;
	struct timeval listen_time;
	struct timeval timeout_time;
};

#endif /* COUNTER_H_ */
