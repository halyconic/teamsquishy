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

#include "utils.h"

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
	char* source_hostname = NULL;
	char* destination_hostname = NULL;
	unsigned int trace_port = 0;
	unsigned int source_port = 0;
	unsigned int destination_port = 0;
	int debug = false;

	while ((cmd = getopt(argc, argv, "a:b:c:d:e:f:")) != -1)
	{
		switch (cmd)
		{
		case 'a':
			trace_port = strtoul(optarg, NULL, 0);
			if (trace_port < 1024 || trace_port > 65536)
			{
				printf("Please supply a port number between 1024 and 65536.");
				return 0;
			}
			break;
		case 'b':
			source_hostname = strdup(optarg);
			break;
		case 'c':
			source_port = strtoul(optarg, NULL, 0);
			if (source_port < 1024 || source_port > 65536)
			{
				printf("Please supply a port number between 1024 and 65536.");
				return 0;
			}
			break;
		case 'd':
			destination_hostname = strdup(optarg);
			break;
		case 'e':
			destination_port = strtoul(optarg, NULL, 0);
			if (destination_port < 1024 ||destination_port > 65536)
			{
				printf("Please supply a port number between 1024 and 65536.");
				return 0;
			}
			break;
		case 'f':
			debug = strtol(optarg, NULL, 0);
			break;
		case '?':
			if (optopt < 'a' || optopt > 'f')
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
	 *
	 * TODO
	 */

	if (trace_port == 0 || source_port == 0 || destination_port == 0)
	{
		printf("Please supply a valid port.\n");
		return 0;
	}

	/*
	 * Setup variables
	 */

	int sock;
	int bytes_read;
	int flags = MSG_DONTWAIT;
	socklen_t addr_len;
	struct sockaddr_in trace_addr, recv_addr;
	struct hostent *src_ent, *dest_ent;

	/*
	 * Resolve network lookups
	 */

	src_ent = (struct hostent *) gethostbyname(source_hostname);
	if ((struct hostent *) src_ent == NULL)
	{
		printf("Host was not found by the name of %s\n", source_hostname);
		exit(1);
	}

	dest_ent = (struct hostent *) gethostbyname(destination_hostname);
	if ((struct hostent *) dest_ent == NULL)
	{
		printf("Host was not found by the name of %s\n", destination_hostname);
		exit(1);
	}

	//printf("%s\n%d\n%x\n", src_ent->h_addr, src_ent->h_addr, src_ent->h_addr);

	// Keep in host order and setup source and destination
	//Address source = Address(src_ent->h_addr, htons(source_port));
	//Address destination = Address(dest_ent->h_addr, destination_port);

	/*
	 * Setup socket
	 */

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	// Own address
	trace_addr.sin_family = AF_INET;
	trace_addr.sin_port = htons(trace_port);
	trace_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(trace_addr.sin_zero), 8);

	// Bind port to listen on
	if (bind(sock, (struct sockaddr *) &trace_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Bind");
		exit(1);
	}

	addr_len = sizeof(struct sockaddr);

	/*
	 * Send
	 */

	/*
	 * Listen
	 */

	/*
	 * Analyze
	 */
}
