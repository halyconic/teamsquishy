/*
 * topology.h
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include <vector>
#include <utility>

#include "utils.h"

struct TopologyEntry
{
	std::vector<Address> entry_vector;
};

void readtopology(char* filename, bool debug);

void createroutes(bool debug);

#endif /* TOPOLOGY_H_ */
