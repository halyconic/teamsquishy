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


#endif /* TOPOLOGY_H_ */
