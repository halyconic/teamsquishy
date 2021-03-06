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

#include <map>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "utils.h"

// Each edge is composed of two uniquely identifying addresses
typedef std::pair<int, int> Edge;
typedef std::pair<int,Address> Vertex;
typedef std::pair<Address, Address> ForwardEntry; // destination, next hop

// Create graph with list structure with identical inbound and outbound links
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;

class GraphManager
{
private:
	int num_vertices;
	int vertex; // Key to vertex_map with your address
	Graph graph;
	std::map<int,Address> vertex_map;
	std::vector<Edge> edge_list;
	std::vector<ForwardEntry> forward_table;

	// Note which edges exist for vertex, and which connections are responsive
	std::vector<std::pair<int, bool> > ports;

	// Parse file into map and edge list
	void create_topology(char* filename, bool debug);

	// Create forwarding table using dijkstra
	void create_forwarding_table(bool debug);

	// Recursively copy
//	Address true_copy_address(const Address &a);
//	Edge true_copy_edge(const Edge &e);

	// Reverse lookup address, returns -1 if not found
	int get_key_from_address(unsigned long int ip_addr, unsigned short int port);

	// Updates routing table if any sizes have changed in graph
	void update_table();

public:

	std::vector<Address> get_port_addresses();

	/*
	 * Record a port as being open
	 */
	void set_port_open(Address address);

	/*
	 * Update port with graphs
	 */
	void update_graph_with_open_ports();

	/*
	 * Recalculate as needed
	 */
	void recalculate(bool debug);

	/*
	 * Outputs to array with routing costs
	 */
	void output_routes(int &node, int* routing_array);

	/*
	 * Interprets an array and updates the routing table if necessary
	 */
	void input_routes(int node, int* routing_array);

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
	GraphManager(char* filename, Address own_address, bool debug);
};

#endif /* GRAPH_H_ */
