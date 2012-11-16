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

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <queue>

#include "forward.h"
#include "packet.h"

class Hop
{
public:
	L2Packet *packet;
	unsigned int next_ip_addr;
	unsigned short int next_port;

	Hop() {;}

	Hop(L2Packet* p, unsigned int i = 0, unsigned short int n = 0) :
		packet(p),
		next_ip_addr(i),
		next_port(n) {;}
};

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
		printf("\n");
	}

	/*
	 * Set up socket
	 */

	int sock;
	int bytes_read; // <- note how this is now on its own line!
	socklen_t addr_len; // <- and this too, with a different type.
	int flags = MSG_DONTWAIT;

	struct sockaddr_in emulator_addr, next_addr;

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
	 * Set up queues
	 */

	const int NUM_QUEUES = 3;
	std::queue<Hop> queues[NUM_QUEUES];

	/*
	 * Listen for incoming packets
	 */

	printf("Emulator polling on port %ld\n", port);
	fflush(stdout);

	L2Packet* recv_packet = new L2Packet();

	while(1)
	{
		bytes_read = recvfrom(sock, *recv_packet, L1_HEADER + L2_HEADER + DEFAULT_PAYLOAD, flags,
				(struct sockaddr *) &next_addr, &addr_len);
		fflush(stdout);

		if (bytes_read >= 0)
		{
			if (debug)
			{
				printf("Packet received:\n");
				recv_packet->print();
				printf("Bytes read: %d\n", bytes_read);
			    printf("Origin: %s %d\n\n",
					   inet_ntoa(next_addr.sin_addr),
					   htons(next_addr.sin_port));
			}

			bool packet_found = false;

			// check forwarding table
			for (unsigned int i = 0; i < forward_table.size(); i++)
			{
				if (recv_packet->dest_ip_addr() == forward_table[i].dest_ip &&
					recv_packet->dest_port() == forward_table[i].dest_port)
				{
					if (debug)
					{
						printf("Matching entry:\n");
						forward_table[i].print();
					}

					packet_found = true;

					unsigned char priority = recv_packet->priority();

					if (priority > 2 || priority == 0)
					{
						// Print error to logstream
						// drop packet
						delete recv_packet;
						recv_packet = new L2Packet();
					}
					else
					{
						if (queues[priority-1].size() < queue_size)
						{
							// add packet
							queues[priority-1].push(Hop(
									recv_packet,
									forward_table[i].next_ip,
									forward_table[i].next_port));
							recv_packet = new L2Packet();
						}
						else
						{
							// drop packet
							delete recv_packet;
							recv_packet = new L2Packet();
						}
					}

					break;
				}
			}

			// Outgoing table entry was not found
			if (!packet_found)
			{
				// drop packet
				delete recv_packet;
				recv_packet = new L2Packet();
			}
		}
		else
		{
			// TODO: implement delay

			bool packet_was_sent = false;
			for (int i = 0; i < NUM_QUEUES; i++)
			{
				if (!queues[i].empty())
				{
					packet_was_sent = true;
					//next_hop = queues[i].pop();
					Hop next_hop = queues[i].front();

					// Next address
					next_addr.sin_family = AF_INET;
					next_addr.sin_port = next_hop.next_port;
					next_addr.sin_addr.s_addr = next_hop.next_ip_addr;
					bzero(&(next_addr.sin_zero), 8);

					if (debug)
					{
						printf("Packet sent:\n");
						next_hop.packet->print();
					    printf("Destination: %s %d\n\n",
							   inet_ntoa(next_addr.sin_addr),
							   htons(next_addr.sin_port));
					}

					sendto(sock, next_hop.packet, next_hop.packet->l2_length(), 0,
							(struct sockaddr *) &next_addr, sizeof(struct sockaddr));

					queues[i].pop();

					break;
				}
			}

			if (!packet_was_sent)
			{

			}
			// if no delay break
			// queue stuff
		}
	}
}
