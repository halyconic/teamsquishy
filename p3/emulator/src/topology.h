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

struct TopologyEntry
{
	std::vector<std::pair<unsigned long int, unsigned short int> > pair;
};

std::vector<TopologyEntry> readtopology(char* filename, bool debug);

#endif /* TOPOLOGY_H_ */
