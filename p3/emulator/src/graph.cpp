/*
 * graph.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#include <stdio.h>
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

	// decrement extra increment and account for line starting at 1
	num_vertices = line - 2;
}

GraphManager::GraphManager(char* filename, unsigned long int ip_addr, unsigned short int port, bool debug)
{
	// Create edge list and vertex map
	create_topology(filename, debug);

	// Identify itself in network, else quit
	vertex = get_key_from_address(ip_addr, port);
//	if (vertex < 0)
//	{
//		printf("Local address does not match node in topology file\n");
//		exit(1);
//	}
	vertex = 1;

    // declare a graph object
    graph = Graph(num_vertices);

    // add the edges to the graph object
    for (unsigned int i = 0; i < edge_list.size(); ++i)
    	boost::add_edge(edge_list[i].first, edge_list[i].second, graph);
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

    // get the property map for vertex indices
	typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
    IndexMap index_map = boost::get(boost::vertex_index, graph);

	typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
	std::pair<vertex_iter, vertex_iter> vp;
	for (vp = boost::vertices(graph); vp.first != vp.second; ++vp.first)
	{
		// Print each vertex
		printf("%lu\n", index_map[*vp.first]);
	}

	printf("\n");

	/*
	 * Print graph as adjacency list
	 */

	printf("All edges:\n");

    using namespace boost;
    graph_traits<Graph>::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei)
    {
    	printf("(%d, %d)\n",
    			index_map[source(*ei, graph)],
       			index_map[target(*ei, graph)]);
    }

	printf("\n");

	/*
	 * Print adjacent nodes
	 */

	printf("Adjacent nodes:\n");

	graph_traits<Graph>::adjacency_iterator ai, ai_end;
	for (tie(ai, ai_end) = adjacent_vertices(vertex, graph); ai != ai_end; ++ai)
	{
    	printf("%d\n",
    			index_map[*ai]);
	}

	printf("\n");
}
