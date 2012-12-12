/*
 * readtopology.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#include <unistd.h> //getopt
#include <ctype.h>  //isprint
#include <stdlib.h> //exit
#include <stdio.h>
#include <string.h> //strdup
#include <arpa/inet.h>
#include <netdb.h>
#include <fstream> //ifstream

#include "topology.h"


std::vector<TopologyEntry> readtopology(char* filename, bool debug)
{

	const int MAX_CHARS_PER_LINE = 512;
	char buffer[MAX_CHARS_PER_LINE];
	const int MAX_TOKENS_PER_LINE = 20;
	const char* const DELIMITER = " ";

	// Stores file into an iterable array
	std::vector<TopologyEntry> topology_entries;
	TopologyEntry topology_entry;
	Address addr;
	unsigned long int ip_int;
	unsigned short int port;
	TopologyEntry current_entry;
	printf("getting into readtopolgy\n");

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

		topology_entries.push_back(topology_entry);

		// update the current entry
		current_entry = topology_entries.back();

		// array to store memory addresses of the tokens in buf
		char* token[MAX_TOKENS_PER_LINE] = {0}; // initialize to 0
		char* pairs[2] = {0};

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

//				printf("first: %lu\n", ip_int);
//				printf("second: %d\n", port);

				topology_entries.back().entry_vector.push_back(Address(ip_int, port));

			}
		}
		printf("\n");
	}

	if (debug)
	{
		// PRINT OUT ENTIRE DATABASE
		printf("ENTIRE TOPOLOGY TABLE\n");
		for (unsigned int i = 0; i < topology_entries.size(); i ++)
		{
			// get the current entry
			TopologyEntry entry = topology_entries.at(i);

			// for each address in the current entry
			for (unsigned int j = 0; j < entry.entry_vector.size(); j++)
			{
				Address addr = entry.entry_vector.at(j);
				printf("%lu,%d ", addr.first, addr.second);
			}

			printf("\n");
		}
	}

	return topology_entries;

}
