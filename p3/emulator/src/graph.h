/*
 * graph.h
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 *
 *  Holds a graph manager that manages the state of links
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "utils.h"

typedef std::pair<int, int> Edge;

class GraphManager
{
	//typedef boost::adjacency_list<vecS, vecS, bidirectionalS> Graph;

private:
	//Graph graph;

public:
	// Returns the next hop given a destination
	Address forward_packet(Address destination);

	// Creates a network given a topology file
	GraphManager(char* filename);
};

#endif /* GRAPH_H_ */
