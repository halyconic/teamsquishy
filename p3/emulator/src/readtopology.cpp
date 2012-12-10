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


void readtopology(char* filename, bool debug)
{
	// Stores file into an iterable array
	std::vector<TopologyEntry> topology_entries;
	printf("getting into readtopolgy\n");

	std::ifstream fin;
	fin.open(filename);
	if (!fin.good())
	{
		printf("Unable to open %s\n", filename);
		exit(-1);
	}

	const int MAX_CHARS_PER_LINE = 512;
	char buffer[MAX_CHARS_PER_LINE];
	const int MAX_TOKENS_PER_LINE = 3;
	const char* const DELIMITER = " ";

	if (debug)
		printf("Input entries:\n");

	/*
	fin.getline(buffer, MAX_CHARS_PER_LINE);



	// array to store memory addresses of the tokens in buf
	char* token_array[MAX_TOKENS_PER_LINE] = {0}; // initialize to 0
	token_array[0] = strtok(buffer, DELIMITER);
	printf("%s\n", token_array[0]);
	token_array[1] = strtok(buffer, DELIMITER);
	printf("%s\n", token_array[1]);
	printf("%s\n", strtok(token_array[0], ","));
	printf("%s\n", strtok(0, ","));*/


	while (!fin.eof())
	{
		//		perror("parsing tokens2\n");
		fflush;

		fin.getline(buffer, MAX_CHARS_PER_LINE);
		//std::vector <std::pair <char*, char*> > pair_list;
		//std::pair <char*, char*> pair;
		std::vector<Address> addr_list;
		topology_entries.push_back(TopologyEntry());
		TopologyEntry topology_entry = topology_entries.back(); // ALIASED, DO NOT USE OUTSIDE LOOP
		Address addr;
		addr_list = topology_entry.entry_vector;

		// array to store memory addresses of the tokens in buf
		char* token[MAX_TOKENS_PER_LINE] = {0}; // initialize to 0

		// parse the line
		token[0] = strtok(buffer, DELIMITER); // first token

		// TODO: fail here if < 4 items
		int n = 0; // for-loop index
		if (token[0]) // zero if line is blank
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
				printf("\n");
			}

			// parse tokens and store into pairs list
			for (int i = 0; i < MAX_TOKENS_PER_LINE; i++)
			{
				//perror("parsing tokens\n");
				//fflush;

				char* temp_first =  strtok(token[i], ",");
				char* test = "223.22.22.21";

				unsigned long int ip_int = inet_addr(temp_first);
				printf("stored ip: %s as unsigned long int: %lu\n", temp_first, ip_int );

				char* temp_second = strtok(0, ",");
				unsigned short int port = (unsigned short) strtoul(temp_second, NULL, 0);

				printf("stored port: %s as unsigned short int: %d\n", temp_second, port);

				addr = Address(ip_int, port);

				if (debug)
					printf("added address to entry_vector with first: %lu and second: %d\n", addr.first, addr.second);

				topology_entry.entry_vector.push_back(Address(ip_int, port));

				if (debug)
					printf("added address to entry_vector");
			}


			printf("\n");
		}


	}

	// for each topology entry in topology entries
	// for each address in topology-entry-vector
	// print first, second
	printf("topology_entries size: %d\n", topology_entries.size());

	for (unsigned int i = 0; i < topology_entries.size(); i++){
		// current entry
		TopologyEntry t = topology_entries.at(i);

		printf("entry_vector size: %d\n", t.entry_vector.size());

		// iterate through each Address in the entry
		for (int j = 0; j < t.entry_vector.size(); j++)
		{
			Address a = t.entry_vector.at(j);
			printf("a: %s b: %s ", a.first, a.second);
		}

		printf("\n");
	}





}
