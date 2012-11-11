/*
 * main.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: KevStev
 */

#include <unistd.h> //getopt
#include <ctype.h>  //isprint
#include <stdlib.h> //exit
#include <stdio.h>

#include "forward.h"

int main(int argc, char **argv)
{
	/*
	 * Handle arguments
	 */

	// If no commands, do nothing
	if (argc <= 1)
	{
		printf("Please supply arguments.\n");
		return 0;
	}

	int cmd;

	char* arg_port = NULL;				//p
	char* arg_queue_size = NULL;		//q
	char* arg_forward_filename = NULL;	//f
	char* arg_log_filename = NULL;		//l

	// Our personal debug options
	bool debug = false;					//d
	char* arg_debug = NULL;				//d

	while ((cmd = getopt(argc, argv, "p:q:f:l:d:")) != -1)
	{
		switch (cmd)
		{
		case 'p':
			arg_port = optarg;
			break;
		case 'q':
			arg_queue_size = optarg;
			break;
		case 'f':
			arg_forward_filename = optarg;
			break;
		case 'l':
			arg_log_filename = optarg;
			break;
		case 'd':
			debug = true;
			arg_debug = optarg;
			break;
		case '?':
			if (optopt == 'p' || optopt == 'q' || optopt == 'f' || optopt == 'l' ||
				optopt == 'd')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				// Better functionality would output the hexadecimal
				fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
			return 0;
			break;
		default:
			// Error!
			exit(-1);
			break;
		}
	}

	/*
	 * Verify all required arguments are supplied here
	 */

	if (!arg_port)
	{
		printf("Please supply a port (Usage: -p <port>).\n");
		return 0;
	}
	else if (!arg_queue_size)
	{
		printf("Please supply a queue size (Usage: -q <queue_size>).\n");
		return 0;
	}
	else if (!arg_forward_filename)
	{
		printf("Please supply a forwarding table filename (Usage: -f <filename>).\n");
		return 0;
	}
	else if (!arg_log_filename)
	{
		printf("Please supply a log filename (Usage: -l <log>).\n");
		return 0;
	}

	/*
	 *  Convert arguments to usable form
	 */

	unsigned long int port = strtoul(arg_port, NULL, 0);
	unsigned long int queue_size = strtoul(arg_queue_size, NULL, 0);

	// Aliases
	char* forward_filename = arg_forward_filename;
	char* log_filename = arg_log_filename;

	/*
	 * Verify variables are within the correct range
	 *
	 * TODO: test new params
	 */

	if (!debug && (port < 1024 || port > 65536))
	{
		printf("Please supply a port number between 1025 and 65535.\n");
		return 0;
	}

	/*
	 * Build forwarding table
	 */

	std::vector<ForwardEntry> forward_table = get_forwarding_table_from_file(forward_filename, debug);

	if (debug)
	{
		printf("Output entries:\n");
		for (unsigned int i = 0; i < forward_table.size(); i++)
			forward_table[i].print();
	}
}
