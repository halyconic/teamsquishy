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
#include "packet.h"

const int MAX_RESEND = 128;

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
		printf("trace_port: %u, source_port: %u, destination_port: %u\n", trace_port, source_port, destination_port);
		printf("Please supply a valid port.\n");
		return 0;
	}

	/*
	 * Setup variables
	 */

	int recv_sock, send_sock;
	int bytes_read;
	int flags = MSG_DONTWAIT;
	socklen_t addr_len;
	struct sockaddr_in trace_addr, send_addr, recv_addr;
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

	Address source = Address((*((struct in_addr *)src_ent->h_addr)).s_addr, htons(source_port));
	Address destination = Address((*((struct in_addr *)dest_ent->h_addr)).s_addr, htons(destination_port));

	if (debug)
	{
		printf("Source address: %s %lu at %d\n",
				inet_ntoa(*((struct in_addr *)src_ent->h_addr)),
				source.first,
				ntohs(source.second),
				source.second);
		printf("Destination address: %s %lu at %d\n",
				inet_ntoa(*((struct in_addr *)src_ent->h_addr)),
				destination.first,
				ntohs(destination.second),
				destination.second);
	}

	/*
	 * Setup socket
	 */

	// Setup port to listen on
	if ((recv_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
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
	if (bind(recv_sock, (struct sockaddr *) &trace_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Bind");
		exit(1);
	}

	addr_len = sizeof(struct sockaddr);

	/*
	 * Set up send
	 */

	// Setup port to send on
	if ((send_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	// Where we are sending to
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(source_port);
	send_addr.sin_addr = *((struct in_addr *)src_ent->h_addr);
	bzero(&(send_addr.sin_zero), 8);

	if (debug)
	{
		printf("Send address: %s %u\n",
			   inet_ntoa(send_addr.sin_addr),
			   ntohs(send_addr.sin_port));
	}

	/*
	 * Setup packets
	 */

	Packet send_packet;
	send_packet.type() = 'T';
	send_packet.TTL() = 0;
	send_packet.set_source(source);
	send_packet.set_destination(destination);

	Packet recv_packet;

	/*
	 * Loop
	 */
	printf("Routetrace probing from port %d\n\n", trace_port);
	fflush(stdout);

	for (unsigned int i = 0; i < MAX_RESEND; i++)
	{
		/*
		 * Send
		 */

		sendto(send_sock, send_packet, HEADER_LENGTH, 0,
				(struct sockaddr *) &send_addr, sizeof(struct sockaddr));

		// TODO Kevin: print out IP and port
		if (debug)
		{
			printf("sending packet:\n");
			send_packet.print();
			printf("actual destination: %s %u\n\n",
				   inet_ntoa(send_addr.sin_addr),
				   ntohs(send_addr.sin_port));
			fflush(stdout);
		}

		/*
		 * Listen
		 */

		bytes_read = recvfrom(recv_sock, recv_packet, HEADER_LENGTH, 0,
			(struct sockaddr *) &recv_addr, &addr_len);

		if (debug && bytes_read <= 0)
		{
			printf("Error! Should never get here.");
			exit(1);
		}

		// TODO Kevin: print out IP and port
		if (debug)
		{
			printf("received packet:\n");
			recv_packet.print();
			printf("actual source: %s %u\n\n",
				   inet_ntoa(recv_addr.sin_addr),
				   ntohs(recv_addr.sin_port));
			fflush(stdout);
		}

		// Possible error!
		if (recv_packet.get_source() == destination)
			break;

		send_packet.TTL()++;
	}
}
