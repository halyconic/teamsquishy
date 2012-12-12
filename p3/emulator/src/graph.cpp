/*
 * graph.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#include <stdio.h>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/config.hpp>

#include "graph.h"
#include "topology.h"

GraphManager::GraphManager(char* filename, bool debug)
{
	/*
	 * Create edge list
	 *
	 * Inefficient, but conceptually easier to understand
	 */

	std::vector<TopologyEntry> entries = readtopology(filename, debug);

	std::vector<Edge> edge_list = std::vector<Edge>();
	for (unsigned int i = 0; i < entries.size(); i++)
	{
		TopologyEntry entry = entries[i];
		Address source = entries[i].entry_vector[0];

		for (unsigned int j = 1; j < entries[i].entry_vector.size(); j++)
		{
//			Address destination = entry.entry_vector[j];
//
//			// Add to graph (worried about pointers here for std::pair)
//			edge_list.push_back(Edge(source, destination));
		}
	}

	// Print all edges
	if (debug)
	{
		printf("Edge list:\n");
		for (std::vector<Edge>::iterator i = edge_list.begin(); i != edge_list.end(); ++i)
		{
			printf("%lu,%d ", i->first.first, i->first.second);
			printf("%lu,%d\n", i->second.first, i->second.second);
		}
		printf("\n");
	}

	/*
	 * Create graph using edge list
	 */

//	graph = Graph(edge_list.begin(), edge_list.end(), entries.size());
}

Address GraphManager::get_next_hop(Address destination, bool debug)
{
//	// Return empty Address
//	return Address(0, 0);
}

std::vector<Address> GraphManager::get_other_hops(Address source, bool debug)
{
//	// Return empty vector
//	return std::vector<Address>();
}

void GraphManager::print_network_info(bool debug)
{
	/*
	 * Print graph as adjacency list
	 */

	printf("All vertices:\n");

//    // get the property map for vertex indices
//	typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
//    IndexMap index_map = boost::get(boost::vertex_index, graph);
//
//	typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
//	std::pair<vertex_iter, vertex_iter> vp;
//	for (vp = boost::vertices(graph); vp.first != vp.second; ++vp.first)
//	{
//		// Print each vertex
//		printf("%lu\n", index_map[*vp.first]);
//	}

	printf("\n");

	/*
	 * Print graph as adjacency list
	 */

	printf("All edges:\n");

    // get the property map for vertex indices
//	typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
//    IndexMap index_map = boost::get(boost::vertex_index, graph);
//
//	typedef boost::graph_traits<Graph>::edge_iterator edge_iter;
//	std::pair<edge_iter, edge_iter> ep;
//	for (ep = boost::vertices(graph); ep.first != ep.second; ++vp.first)
//	{
//		// Print each vertex
//		printf("(%lu, %lu)\n", index_map[source(*ei, g)]);
//	}

	printf("\n");

	/*
	 * Print adjacent nodes
	 */

	printf("Adjacent nodes:\n");

//	graph.
//
//	for (vp = boost::vertices(graph); vp.first != vp.second; ++vp.first)
//	{
//		// Print each vertex
//		printf("%lu\n", index_map[*vp.first]);
//	}

	printf("\n");
}
