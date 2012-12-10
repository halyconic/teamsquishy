/*
 * main.cpp
 *
 *  Created on: Dec 8, 2012
 *      Author: KevStev
 */

#include <unistd.h> //getopt
#include <ctype.h>  //isprint
#include <stdlib.h> //exit
#include <stdio.h>
#include <string.h> //strdup
#include <arpa/inet.h>
#include <netdb.h>

#include "packet.h"
#include "topology.h"

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

	// Variables
	char* filename = NULL;
	unsigned int port = 0;
	bool debug = false;

	while ((cmd = getopt(argc, argv, "p:f:d")) != -1)
	{
		switch (cmd)
		{
		case 'p':
			port = strtoul(optarg, NULL, 0);
			if (port < 1024 || port > 65536)
			{
				printf("Please supply a port number between 1024 and 65536.");
				return 0;
			}
			break;
		case 'f':
			filename = strdup(optarg);
			break;
		case 'd':
			debug = true;
			break;
		case '?':
			if (optopt == 'p' || optopt == 'f')
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

	if (port == 0)
	{
		printf("Please supply a valid port (Usage: -p <port>).\n");
		return 0;
	}
	else if (!filename)
	{
		printf("Please supply a filename (Usage: -f <filename>).\n");
		return 0;
	}

	/*
	 * Build forwarding table
	 */

	//std::vector<TopologyEntry> topology = readtopology(filename, debug);
	readtopology(filename, debug);

	/*
	 * Setup variables
	 */

	int sock;
	int bytes_read;
	int flags = MSG_DONTWAIT;
	socklen_t addr_len;
	struct sockaddr_in emulator_addr, recv_addr, next_addr;

	/*
	 * Setup socket
	 */

	// Own address
	emulator_addr.sin_family = AF_INET;
	emulator_addr.sin_port = htons(port);
	emulator_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(emulator_addr.sin_zero), 8);

	// Bind port to listen on
	if (bind(sock, (struct sockaddr *) &emulator_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Bind");
		exit(1);
	}

	addr_len = sizeof(struct sockaddr);

	/*
	 * Listen for incoming packets
	 */

	printf("Emulator polling on port %d\n", port);
	fflush(stdout);
	Packet *recv_packet;

	while(1)
	{
		/*
		 * Create routes
		 */

		// createroutes();

		/*
		 * Listen
		 */
		bytes_read = recvfrom(sock, *recv_packet, HEADER_LENGTH, flags,
						(struct sockaddr *) &recv_addr, &addr_len);
	}
}
