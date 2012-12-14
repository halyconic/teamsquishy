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
#include <arpa/inet.h>
#include <netdb.h>
#include <fstream> //ifstream

#include "graph.h"

int GraphManager::get_key_from_address(unsigned long int a, unsigned short int p)
{
	int key = -1;

	for (std::map<int,Address>::iterator i = vertex_map.begin();
			i != vertex_map.end();
			++i)
	{
		if (i->second.first == a && i->second.second == p)
		{
			key = i->first;
			break;
		}
	}

	return key;
}

void GraphManager::create_topology(char* filename, bool debug)
{
	const int MAX_CHARS_PER_LINE = 512;
	char buffer[MAX_CHARS_PER_LINE];
	const int MAX_TOKENS_PER_LINE = 20;
	const char* const DELIMITER = " ";

	// Stores file into an iterable array
	unsigned long int ip_int = 0;
	unsigned short int port = 0;
	int line = 1;
	int left_edge = -1;
	int right_edge = -1;

	// BEGIN FILE I/O
	std::ifstream fin;
	fin.open(filename);
	if (!fin.good())
	{
		printf("Unable to open %s\n", filename);
		exit(-1);
	}

	// LOOP THROUGH EACH LINE
	while (!fin.eof())
	{
		fin.getline(buffer, MAX_CHARS_PER_LINE);

		// array to store memory addresses of the tokens in buf
		char* token[MAX_TOKENS_PER_LINE] = {0}; // initialize to 0
		//char* pairs[2] = {0};

		// parse the line
		token[0] = strtok(buffer, DELIMITER); // first token

		int n = 0;
		if (token[0])
		{
			// get the rest of the tokens
			for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
			{
				token[n] = strtok(0, DELIMITER); // subsequent tokens
				if (!token[n])
					break; // no more tokens
			}

			// Print file as inputting
			if (debug)
			{
				for (int i = 0; i < MAX_TOKENS_PER_LINE; i++)
				{
					printf(token[i]);
					printf(" ");
				}
			}
		}

		for (int j = 0; j < MAX_TOKENS_PER_LINE; j++)
		{
			if (token[j] != NULL)
			{
				char* temp_ip = strtok(token[j], ",");
				char* temp_port = strtok(0, ",");

				ip_int = inet_addr(temp_ip);
				port = (unsigned short) strtoul(temp_port, NULL, 0);

				// Add to vertex list
				if (j == 0)
				{
					// Adding an extra vertex here at end, beware!
					vertex_map.insert(Vertex(line, Address(ip_int, port)));

					// Store current edge
					left_edge = line;
				}
				else
				{
					right_edge = get_key_from_address(ip_int, port);

					if (right_edge >= 0)
					{
						edge_list.push_back(Edge(left_edge, right_edge));
					}
				}
			}
		}
		printf("\n");
		line++;
	}

	if (debug)
	{
		printf("All initial vertices:\n");
		for (std::map<int,Address>::iterator i = vertex_map.begin();
				i != vertex_map.end();
				++i)
		{
			printf("%d: %lu, %u\n", i->first, i->second.first, i->second.second);
		}
		printf("\n");

		printf("All initial edges:\n");
		for (std::vector<Edge>::iterator i = edge_list.begin();
				i != edge_list.end();
				++i)
		{
			printf("(%d, %d)\n", i->first, i->second);
		}
		printf("\n");
	}
}

GraphManager::GraphManager(char* filename, bool debug)
{
	/*
	 * Create edge list
	 *
	 * Inefficient, but conceptually easier to understand
	 */

	create_topology(filename, debug);

//	std::vector<Edge> edge_list = std::vector<Edge>();
//	for (unsigned int i = 0; i < entries.size(); i++)
//	{
//		TopologyEntry entry = entries[i];
//		Address source = entries[i].entry_vector[0];
//
//		// Insert into edge list
//		for (unsigned int j = 1; j < entries[i].entry_vector.size(); j++)
//		{
//			Address destination = entry.entry_vector[j];
//
//			if (0 && debug)
//				printf("pushing back a new edge to the edge list correctly!\n");
//
//			edge_list.push_back(Edge(true_copy_address(entries[i].entry_vector[0]), true_copy_address(entry.entry_vector[j])));
//		}
//	}
//
//	// Print all edges
//	if (debug)
//	{
//		printf("Edge list:\n");
//		for (std::vector<Edge>::iterator i = edge_list.begin(); i != edge_list.end(); ++i)
//		{
//			printf("%lu,%d ", i->first.first, i->first.second);
//			printf("%lu,%d\n", i->second.first, i->second.second);
//		}
//		printf("\n");
//	}

	/*
	 * Create graph using edge list
	 */

//	graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);

//	std::vector<Edge>::iterator edge_list_begin = edge_list.begin();
//	std::vector<Edge>::iterator edge_list_end = edge_list.end();

//	graph = Graph(edge_list.begin, edge_list.end, entries.size());

//	typedef std::pair<int, int> Temp;
//	std::vector<Temp> temp_list;
//
//	Graph graph(edge_list.begin(), edge_list.end(), edge_list.size());

    // declare a graph object
//    Graph g(num_vertices);
//
//    // add the edges to the graph object
//    for (int i = 0; i < num_edges; ++i)
//      add_edge(edge_array[i].first, edge_array[i].second, g);
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
