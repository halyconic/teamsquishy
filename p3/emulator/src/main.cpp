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

#include "counter.h"
#include "packet.h"
#include "graph.h"

const int START_TTL = 128;

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
	 * Setup variables
	 */

	int sock, test_sock;
	int bytes_read;
	int flags = MSG_DONTWAIT;
	socklen_t addr_len;
	struct sockaddr_in emulator_addr, recv_addr, next_addr, test_addr;

	/*
	 * Setup socket
	 */

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

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
	 * Cache current location
	 */

	if ((test_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

    const char* google_dns_ip = "8.8.8.8";
    uint16_t dns_port = 53;
    struct sockaddr_in lookup_serv_addr;
    memset(&lookup_serv_addr, 0, sizeof(lookup_serv_addr));
    lookup_serv_addr.sin_family = AF_INET;
    lookup_serv_addr.sin_addr.s_addr = inet_addr(google_dns_ip);
    lookup_serv_addr.sin_port = htons(dns_port);

    int err = connect(test_sock, (const sockaddr*) &lookup_serv_addr, sizeof(lookup_serv_addr));
    // TODO: verify err != -1

    socklen_t namelen = sizeof(test_addr);
    err = getsockname(test_sock, (sockaddr*) &test_addr, &namelen);
    // TODO: verify err != -1

    close(test_sock);

    // Set current port
    test_addr.sin_port = htons(port);

    if (debug)
	{
		printf("Own address: %s %u\n",
			   inet_ntoa(test_addr.sin_addr),
			   ntohs(test_addr.sin_port));
	}

    Address emulator_address = Address(
    		(unsigned long int) test_addr.sin_addr.s_addr,
    		(unsigned short int) test_addr.sin_port);

	if (debug)
		printf("Our address: %lu, %u\n\n", emulator_address.first, emulator_address.second);

	/*
	 * Build forwarding table
	 */

	GraphManager graph_manager(filename, emulator_address, debug);
	graph_manager.print_network_info(debug);

	/*
	 * Listen for incoming packets
	 */

	printf("Emulator polling on port %d\n\n", port);
	fflush(stdout);
	RoutePacket recv_packet;
	RoutePacket send_packet;

	send_packet.type() = 'T';
	send_packet.TTL() = 0;
	send_packet.set_source(emulator_address);


	Counter c;
	char* route_arrays;
	int seq_no;

	Address current_hop_address;
	std::vector<Address> other_hops_vector;
	int send_sock;

	// Setup port to send on
	if ((send_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}


	while (1)
	{
		/*
		 * Explore every 4 seconds
		 */

		// createroutes();
		// forward the packet

		other_hops_vector = graph_manager.get_other_hops(emulator_address, debug);

		// sets up the payload for packet to be sent
		route_arrays = send_packet.route_array();
		seq_no = graph_manager.output_routes(route_arrays);
		send_packet.sequence_number() = seq_no;

		for (int i = 0; i < other_hops_vector.size(); i++)
		{
			current_hop_address = other_hops_vector.at(i);

			next_addr.sin_family = AF_INET;
			next_addr.sin_port = current_hop_address.second;
			next_addr.sin_addr.s_addr = current_hop_address.first;
			send_packet.set_destination(current_hop_address);
			bzero(&(next_addr.sin_zero), 8);

			sendto(send_sock, send_packet, HEADER_LENGTH, 0,
									(struct sockaddr *) &next_addr, sizeof(struct sockaddr));
		}

		/*
		 * Listen
		 */
		while(!c.check())
		{
			bytes_read = recvfrom(sock, recv_packet, HEADER_LENGTH, flags,
							(struct sockaddr *) &recv_addr, &addr_len);

			// Packet received
			if (bytes_read >= 0)
			{
				if (debug)
				{
					printf("received packet:\n");
					recv_packet.print();
					printf("actual source: %lu %u (%s %u)\n\n",
							recv_addr.sin_addr.s_addr,
							recv_addr.sin_port,
							inet_ntoa(recv_addr.sin_addr),
							ntohs(recv_addr.sin_port));
					fflush(stdout);
				}

				if (recv_packet.TTL() <= 0)
				{
					// Send to routetrace
					if (recv_packet.type() == 'T')
					{
						recv_packet.TTL() = START_TTL;

						// Send to next shortest path
						next_addr.sin_family = AF_INET;
						next_addr.sin_port = recv_packet.get_source().second;
						next_addr.sin_addr.s_addr = recv_packet.get_source().first;
						bzero(&(next_addr.sin_zero), 8);

						// Set own address in source field for trace
						recv_packet.set_source(emulator_address);

						sendto(send_sock, recv_packet, HEADER_LENGTH, 0,
								(struct sockaddr *) &next_addr, sizeof(struct sockaddr));

						if (debug)
						{
							printf("sending packet:\n");
							recv_packet.print();
							printf("actual destination: %lu %u (%s %u)\n\n",
									next_addr.sin_addr.s_addr,
									next_addr.sin_port,
									inet_ntoa(next_addr.sin_addr),
									ntohs(next_addr.sin_port));
							fflush(stdout);
						}
					}
					else
					{
						// Drop packet
						recv_packet.clear();
					}

					if (debug && recv_packet.TTL() < 0)
					{
						printf("Error! Should never get here.");
						exit(1);
					}
				}
				else
				{
					recv_packet.TTL()--;

					if (recv_packet.type() == 'T')
					{
						Address current_hop_address = graph_manager.get_next_hop(recv_packet.get_destination(), debug);

						// If node predecessor is yourself send directly to destination
						if (current_hop_address == emulator_address)
						{
							current_hop_address = recv_packet.get_destination();
						}

						// Send to next shortest path
						next_addr.sin_family = AF_INET;
						next_addr.sin_port = current_hop_address.second;
						next_addr.sin_addr.s_addr = current_hop_address.first;
						bzero(&(next_addr.sin_zero), 8);

						sendto(send_sock, recv_packet, HEADER_LENGTH, 0,
								(struct sockaddr *) &next_addr, sizeof(struct sockaddr));

						if (debug)
						{
							printf("sending packet:\n");
							recv_packet.print();
							printf("actual destination: %lu %u (%s %u)\n\n",
									next_addr.sin_addr.s_addr,
									next_addr.sin_port,
									inet_ntoa(next_addr.sin_addr),
									ntohs(next_addr.sin_port));
							fflush(stdout);
						}
					}
					else if (recv_packet.type() == 'R')
					{
						// TODO: do something more...
						printf("packet is of type R...do something cooler\n");
					}
				}
			}
		}
	}
}
