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

// Each edge is composed of two uniquely identifying addresses
typedef std::pair<Address, Address> Edge;

// Create graph with list structure with identical inbound and outbound links
typedef boost::adjacency_list<
		boost::listS,
		boost::listS,
		boost::undirectedS,
		boost::property<boost::vertex_index_t, std::size_t>
		> Graph;

class GraphManager
{
private:
	Graph graph;

public:
	/*
	 * Returns the next hop given a destination
	 */
	Address get_next_hop(Address destination, bool debug);

	/*
	 * Given an adjacent node, return all other nodes this node is connected to
	 *
	 * Returns an empty vector is the source node is not connected
	 */
	std::vector<Address> get_other_hops(Address source, bool debug);

	/*
	 * Print network statistics
	 */
	void print_network_info(bool debug);

	/*
	 * Creates a network graph given a topology file
	 */
	GraphManager(char* filename, bool debug);
};

#endif /* GRAPH_H_ */
