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
#include "topology.h"
#include <fstream> //ifstream
#include <stdio.h>
#include <string.h>
#include <stdlib.h> //exit

void readtopology(char* filename, bool debug)
{
	// Stores file into an iterable array
	std::vector<TopologyEntry> topologyEntries;
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

		fin.getline(buffer, MAX_CHARS_PER_LINE);
		std::vector <std::pair <char*, char*> > pair_list;
		std::pair <char*, char*> pair;

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
				pair.first = strtok(token[i], ",");
				pair.second = strtok(0, ",");
				printf("%s -- %s,", pair.first, pair.second);
				printf(" ");
			}

			printf("\n");

			/*	//printf("the first token: %s\n", token[0]);
			topologyEntries.push_back(TrackerEntry(
					strdup(token[0]),
					strtoul(token[1], NULL, 0),
					strdup(token[2]),
					strtoul(token[3], NULL, 0)))*/;
			TopologyEntry entry;
			//topologyEntries.push_back(entry(token));
		}
	}
	/*	// parse the line into tokens, stor into token
	for (int i = 0; 2; i ++)
	{
		// store the current token
		token_array[i] = strtok(buffer, DELIMITER);

		if (debug)
			printf("token %d: %s", i, token_array[i]);

		printf("hi\n");

	}*/




}
