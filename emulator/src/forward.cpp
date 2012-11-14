/*
 * forward.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: swebber
 */

#include "forward.h"

#include <fstream> //ifstream
#include <stdio.h>
#include <string.h>
#include <stdlib.h> //exit

const char* domain = ".cs.wisc.edu";

ForwardEntry::ForwardEntry(
		char* e_h, unsigned short int e_p,
		char* d_h, unsigned short int d_p,
		char* n_h, unsigned short int n_p,
		unsigned int d, unsigned char l) :
	emu_port(e_p),
	dest_port(d_p),
	next_port(n_p),
	delay(d),
	loss(l)
{
	// create a buffer for ip_lookup
	char* ip_lookup;
	struct hostent * ip_buffer;

	// set values
	ip_lookup = strcat(e_h, domain);
	ip_buffer = gethostbyname(ip_lookup);
	if ((struct hostent *) ip_buffer == NULL)
	{
		// TODO: Gracefully handle missing sender
		printf("Host was not found by the name of %s\n", e_h);
		exit(1);
	}
	//printf("%o\n", ((*((struct in_addr *)ip_buffer->h_addr)).s_addr));
	emu_ip = (unsigned int)(*((struct in_addr *)ip_buffer->h_addr)).s_addr;
	ip_lookup = strcat(d_h, domain);
	ip_buffer = (struct hostent *) gethostbyname(ip_lookup);
	if ((struct hostent *) ip_buffer == NULL)
	{
		// TODO: Gracefully handle missing sender
		printf("Host was not found by the name of %s\n", e_h);
		exit(1);
	}
	//printf("%o\n", (*((struct in_addr *)ip_buffer->h_addr)).s_addr);
	dest_ip = (unsigned int)(*((struct in_addr *)ip_buffer->h_addr)).s_addr;
	ip_lookup = strcat(n_h, domain);
	ip_buffer = (struct hostent *) gethostbyname(ip_lookup);
	if ((struct hostent *) ip_buffer == NULL)
	{
		// TODO: Gracefully handle missing sender
		printf("Host was not found by the name of %s\n", e_h);
		exit(1);
	}
	//printf("%o\n", (*((struct in_addr *)ip_buffer->h_addr)).s_addr);
	next_ip = (unsigned int)(*((struct in_addr *)ip_buffer->h_addr)).s_addr;
	ip_lookup = "127.0.0.1";
	ip_buffer = gethostbyname(ip_lookup);
	if ((struct hostent *) ip_buffer == NULL)
	{
		// TODO: Gracefully handle missing sender
		printf("Host was not found by the name of %s\n", e_h);
		exit(1);
	}
	//printf("%o\n", ((*((struct in_addr *)ip_buffer->h_addr)).s_addr));
	ip_lookup = "1.1.1.1";
	ip_buffer = gethostbyname(ip_lookup);
	if ((struct hostent *) ip_buffer == NULL)
	{
		// TODO: Gracefully handle missing sender
		printf("Host was not found by the name of %s\n", e_h);
		exit(1);
	}
	//printf("%o\n", ((*((struct in_addr *)ip_buffer->h_addr)).s_addr));
}

std::vector<ForwardEntry> get_forwarding_table_from_file(char* filename, bool debug)
{
	// Stores file into an iterable array
	std::vector<ForwardEntry> tracker;

	std::ifstream fin;
	fin.open(filename);
	if (!fin.good())
	{
		printf("Unable to open %s\n", filename);
		exit(-1);
	}

	const int MAX_CHARS_PER_LINE = 512;
	char buffer[MAX_CHARS_PER_LINE];
	const int MAX_TOKENS_PER_LINE = 8;
	const char* const DELIMITER = " ";

	if (0 && debug)
		printf("Input entries:\n");

	// read each line of the file
	while (!fin.eof())
	{
		fin.getline(buffer, MAX_CHARS_PER_LINE);

		// array to store memory addresses of the tokens in buf
		char* token[MAX_TOKENS_PER_LINE] = {0}; // initialize to 0

		// parse the line
		token[0] = strtok(buffer, DELIMITER); // first token

		// TODO: fail here if < 8 items
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
			if (0 && debug)
			{
				for (int i = 0; i < MAX_TOKENS_PER_LINE; i++)
				{
					printf(token[i]);
					printf(" ");
				}
				printf("\n");
			}

			//printf("the first token: %s\n", token[0]);
			tracker.push_back(ForwardEntry(
					strdup(token[0]),
					strtoul(token[1], NULL, 0),
					strdup(token[2]),
					strtoul(token[3], NULL, 0),
					strdup(token[4]),
					strtoul(token[5], NULL, 0),
					strtoul(token[6], NULL, 0),
					strtoul(token[7], NULL, 0)));
		}
	}

	//printf("TRACKER SIZE: %d", tracker.size());

	return tracker;
}
