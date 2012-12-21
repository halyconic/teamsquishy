/*
 * graph.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#include <stdio.h>
#include <boost/graph/breadth_first_search.hpp>
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
				port = htons((unsigned short) strtoul(temp_port, NULL, 0));

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

void GraphManager::create_forwarding_table(bool debug)
{
	using namespace boost;

	typedef property_map<Graph, vertex_index_t>::type IndexMap;
	    IndexMap index_map = get(vertex_index, graph);

	std::vector<int> p(boost::num_vertices(graph));

	breadth_first_search(graph, index_map[vertex], visitor(
			make_bfs_visitor(
					record_predecessors(&p[0], on_tree_edge()))));

	forward_table.clear();

	if (debug)
		printf("Next hop for each node from %d:\n", vertex);

	graph_traits<Graph>::vertex_iterator vi;
	for(vi = vertices(graph).first; vi != vertices(graph).second; ++vi)
	{
		int d = index_map[*vi];
		int n = p[*vi];

		if (debug)
			printf("Dest: %d Hop: %d\n", d, n);

		forward_table.push_back(ForwardEntry(vertex_map[d], vertex_map[n]));
	}

	if (debug)
		printf("\n");

	// Verify forwarding table works
	if (debug)
	{
		printf("Forwarding table:\n");
		std::vector<ForwardEntry>::iterator fp;
		for (fp = forward_table.begin(); fp != forward_table.end(); ++fp)
		{
			// Print each vertex
			printf("(%lu, %u) (%lu, %u)\n", fp->first.first, fp->first.second, fp->second.first, fp->second.second);
		}
		printf("\n");
	}
}

GraphManager::GraphManager(char* filename, Address own_address, bool debug)
{
	// Create edge list and vertex map
	create_topology(filename, debug);

	// Identify itself in network, else quit
	vertex = get_key_from_address(own_address.first, own_address.second);
	if (vertex < 0)
	{
		printf("Local address does not match node in topology file\n");
		exit(1);
	}

	// Add emulated port that pings can occur across
	for (unsigned int i = 0; i < edge_list.size(); ++i)
	{
		if (edge_list[i].first == vertex)
			ports.push_back(std::pair<int, bool>(edge_list[i].second, false));
		else if (edge_list[i].second == vertex)
			ports.push_back(std::pair<int, bool>(edge_list[i].first, false));
	}

    // declare a graph object
    graph = Graph(num_vertices);

    // add the edges to the graph object
    for (unsigned int i = 0; i < edge_list.size(); ++i)
    	boost::add_edge(edge_list[i].first, edge_list[i].second, graph);

    create_forwarding_table(debug);
}

Address GraphManager::get_next_hop(Address destination, bool debug)
{
	Address hop = Address(0, 0);

	// Search forwarding table
	std::vector<ForwardEntry>::iterator fp;
	for (fp = forward_table.begin(); fp != forward_table.end(); ++fp)
	{
		// Set hop, worried about memory here
		if (fp->first == destination)
		{
			if (debug)
				printf("(%lu, %u) -> (%lu, %u)\n", fp->first.first, fp->first.second, fp->second.first, fp->second.second);
			hop = fp->second;
		}
	}

	return hop;
}

std::vector<Address> GraphManager::get_other_hops(Address source, bool debug)
{
	std::vector<Address> hops;

	if (debug)
		printf("Getting hops:");

	using namespace boost;
	typedef property_map<Graph, vertex_index_t>::type IndexMap;
    IndexMap index_map = get(vertex_index, graph);
	graph_traits<Graph>::adjacency_iterator ai, ai_end;
	for (tie(ai, ai_end) = adjacent_vertices(vertex, graph); ai != ai_end; ++ai)
	{
		int key = index_map[*ai];

		if (debug)
			printf(" %d", key);

		hops.push_back(vertex_map[key]);
	}

	if (debug)
		printf("\n");

	// Could have memory issues if vertex_map is deleted while being used
	return hops;
}

/*
 * Outputs open ports to array
 */
void GraphManager::output_routes(int &node, int* routing_array)
{
	node = vertex;

	unsigned int routing_iter = 0;

	for (unsigned int i = 0; i < ports.size(); ++i)
	{
		if (ports[i].second == true)
		{
			routing_array[routing_iter] = ports[i].first;
			++i;
		}
	}
}

void GraphManager::input_routes(int node, int* routing_array)
{
	using namespace boost;

	typedef property_map<Graph, vertex_index_t>::type IndexMap;
	    IndexMap index_map = get(vertex_index, graph);

	typedef graph_traits<Graph>::vertex_iterator vertex_iter;

	printf("vertex %d", vertex);

	// TODO: does this work?
	clear_vertex(node, graph);

	print_network_info(true);

//	for (unsigned int i = 0; routing_array[i] != 0; ++i)
//	{
//		add_edge(node, routing_array[i], graph);
//	}
//
//	print_network_info(true);
}

std::vector<Address> GraphManager::get_port_addresses()
{
	std::vector<Address> port_addresses;

	for (unsigned int i = 0; i < ports.size(); ++i)
	{
		ports[i].second = false;
		port_addresses.push_back(vertex_map[ports[i].first]);
	}

	return port_addresses;
}

void GraphManager::set_port_open(Address address)
{
	int key = get_key_from_address(address.first, address.second);

	for (unsigned int i = 0; i < ports.size(); ++i)
	{
		if (ports[i].first == key)
		{
			ports[i].second = true;

			// One match only
			break;
		}
	}
}

void GraphManager::recalculate(bool debug)
{
	create_forwarding_table(debug);
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
    	printf("(%ld, %ld)\n",
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
    	printf("%ld\n",
    			index_map[*ai]);
	}

	printf("\n");
}
