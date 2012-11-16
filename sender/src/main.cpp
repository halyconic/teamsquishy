/*
 * main.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: stephen
 */

#include <unistd.h> //getopt
#include <ctype.h>  //isprint
#include <stdlib.h> //exit
#include <stdio.h>
#include <vector>
#include <algorithm>//sort
#include <istream>

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <errno.h>
#include <string.h>
#include <cstdlib> //strtol

#include "packet.h"
#include "counter.h"

const char* domain = ".cs.wisc.edu";

/*
 * TODO:
 *   Comment!
 */
int main(int argc, char **argv)
{
	// Handle arguments

	// If no commands, do nothing
	if (argc <= 1)
	{
		printf("Please supply arguments.\n");
		return 0;
	}

	int cmd;

	char* arg_sender_port = NULL;		//p
	char* arg_requester_port = NULL;	//g
	char* arg_rate = NULL;				//r
	char* arg_seq_no = NULL;			//q
	char* arg_length = NULL;			//l
	char* arg_emu_hostname = NULL;		//f
	char* arg_emu_port = NULL;			//h
	char* arg_priority = NULL;			//i
	char* arg_timeout = NULL;			//t

	// Our personal debug options
	bool debug = false;					//d
	char* arg_debug = NULL;				//d

	while ((cmd = getopt(argc, argv, "p:g:r:q:l:f:h:i:t:d:")) != -1)
	{
		switch (cmd)
		{
		case 'p':
			arg_sender_port = optarg;
			break;
		case 'g':
			arg_requester_port = optarg;
			break;
		case 'r':
			arg_rate = optarg;
			break;
		case 'q':
			arg_seq_no = optarg;
			break;
		case 'l':
			arg_length = optarg;
			break;
		case 'f':
			arg_emu_hostname = optarg;
			break;
		case 'h':
			arg_emu_port = optarg;
			break;
		case 'i':
			arg_priority = optarg;
			break;
		case 't':
			arg_timeout = optarg;
			break;
		case 'd':
			debug = true;
			arg_debug = optarg;
			break;
		case '?':
			if (optopt == 'p' || optopt == 'g' || optopt == 'r' || optopt == 'q' || optopt == 'l' ||
				optopt == 'f' || optopt == 'h' || optopt == 'i' || optopt == 't' ||
				optopt == 'd')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				// Better functionality would output the hexadecimal
				fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			return 0;
			break;
		default:
			// Error!
			exit(-1);
			break;
		}
	}

	// Verify all required arguments are supplied here

	if (!arg_sender_port)
	{
		printf("Please supply a sender port (Usage: -p <port>).\n");
		return 0;
	}
	else if (!arg_requester_port)
	{
		printf("Please supply a requester port (Usage: -g <port>).\n");
		return 0;
	}
	else if (!arg_rate)
	{
		printf("Please supply a rate (Usage: -r <rate>).\n");
		return 0;
	}
	else if (!arg_seq_no)
	{
		printf("Please supply a sequence number (Usage: -q <seq_no>).\n");
		return 0;
	}
	else if (!arg_length)
	{
		printf("Please supply a length (Usage: -l <length>).\n");
		return 0;
	}
	else if (!arg_emu_hostname)
	{
		printf("Please supply a forwarding hostname (Usage: -f <f_hostname>).\n");
		return 0;
	}
	else if (!arg_emu_port)
	{
		printf("Please supply a forwarding port (Usage: -h <f_port>).\n");
		return 0;
	}
	else if (!arg_priority)
	{
		printf("Please supply a forwarding hostname (Usage: -f <f_hostname>).\n");
		return 0;
	}
	else if (!arg_timeout)
	{
		printf("Please supply a forwarding port (Usage: -h <f_port>).\n");
		return 0;
	}

	/*
	 * Convert arguments to usable form
	 */

	unsigned long int sender_port = strtoul(arg_sender_port, NULL, 0);
	unsigned long int requester_port = strtoul(arg_requester_port, NULL, 0);
	unsigned long int emu_port = strtoul(arg_emu_port, NULL, 0);
	double rate = strtod(arg_rate, NULL);
	unsigned long int seq_no = strtoul(arg_seq_no, NULL, 0);
	unsigned long int length = strtoul(arg_length, NULL, 0);
	unsigned long int priority = strtoul(arg_priority, NULL, 0);
	unsigned long int timeout = strtoul(arg_timeout, NULL, 0);

	// Aliases
	char* emu_hostname = arg_emu_hostname;

	/*
	 * Verify variables are within the correct range
	 *
	 * TODO: test new params
	 */

	if (sender_port < 1024 || sender_port > 65536
			|| requester_port < 1024 || requester_port > 65536
			|| emu_port < 1024 || emu_port > 65536)
	{
		printf("Please supply port numbers between 1025 and 65535.");
		return 0;
	}
	if (priority < 1 || priority > 3)
	{
		printf("Please supply a priority between 1 and 3.");
		return 0;
	}

	/*
	 * Declare useful variables
	 */

	int sock, curr_sock;
	int bytes_read; // <- note how this is now on its own line!
	socklen_t addr_len; // <- and this too, with a different type.
	struct sockaddr_in requester_addr, sender_addr, emu_addr, curr_addr;
	struct hostent *emu_ent;

	/*
	 * Cache current location
	 */

	if ((curr_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
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

    int err = connect(curr_sock, (const sockaddr*) &lookup_serv_addr, sizeof(lookup_serv_addr));
    // TODO: verify err != -1

    socklen_t namelen = sizeof(curr_addr);
    err = getsockname(curr_sock, (sockaddr*) &curr_addr, &namelen);
    // TODO: verify err != -1

    if (debug)
	{
		printf("Own address: %s %u\n",
			   inet_ntoa(curr_addr.sin_addr),
			   ntohs(curr_addr.sin_port));
	}
	close(curr_sock);

	/*
	 * Set up send
	 */

	// Intermediate emulator destination
	char* ip_lookup = new char[strlen(emu_hostname) + strlen(domain)];
	ip_lookup = strcat(emu_hostname, domain);
	emu_ent = (struct hostent *) gethostbyname(ip_lookup);

	// Verify emulator exists
	if ((struct hostent *) emu_ent == NULL)
	{
		// TODO: Gracefully handle missing sender
		printf("Host was not found by the name of %s\n", emu_hostname);
		exit(1);
	}

	emu_addr.sin_family = AF_INET;
	emu_addr.sin_port = htons(emu_port);
	emu_addr.sin_addr = *((struct in_addr *)emu_ent->h_addr);
	bzero(&(emu_addr.sin_zero), 8);

	if (0 && debug)
	{
		printf("IP emulator lookup: %s\n", ip_lookup);
	    printf("Next hop: %s %u\n",
			   inet_ntoa(emu_addr.sin_addr),
			   ntohs(emu_addr.sin_port));
	}

	/*
	 * Set up receive
	 */

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	// Own address
	sender_addr.sin_family = AF_INET;
	sender_addr.sin_port = htons(sender_port);
	sender_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(sender_addr.sin_zero), 8);

	// Bind port to listen on
	if (bind(sock, (struct sockaddr *) &sender_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Bind");
		exit(1);
	}

	addr_len = sizeof(struct sockaddr);

	/*
	 * Listen for incoming requests
	 */

	printf("Sender waiting for requester on port %ld\n", sender_port);
	fflush(stdout);

	L2Packet recv_packet;

	bytes_read = recvfrom(sock, recv_packet, recv_packet.l2_length(), 0,
		(struct sockaddr *) &requester_addr, &addr_len);

	if (debug)
	{
		printf("Packet being received:\n");
		recv_packet.print();
		printf("Bytes read: %d\n", bytes_read);
		printf("Origin: %s %u\n\n",
			   inet_ntoa(requester_addr.sin_addr),
			   ntohs(requester_addr.sin_port));
		fflush(stdout);
	}

	// Set destination port of requester
	requester_addr.sin_port = htons(requester_port);

	if (recv_packet.type() == 'R')
	{
		// Create window
		if (recv_packet.length() <= 0)
		{
			printf("The receiver has requested a packet window less than size 1.\n");
			printf("Request has been ignored.\n");
			exit(0);
		}
		unsigned int window_size = recv_packet.length();

		// Break requested file into packets
		if (debug)
			printf("Filename: %s\n", recv_packet.payload());

		std::ifstream::pos_type file_size;
		std::ifstream filestr;

		filestr.open (recv_packet.payload(), std::ios::in|std::ios::binary|std::ios::ate);

		if (filestr.is_open() == false)
		{
			printf("Unable to open file %s\n", recv_packet.payload());
			exit(-1);
		}

		file_size = filestr.tellg();
		filestr.seekg(0, std::ios::beg);

		Counter counter = Counter(rate);

		L2Packet* send_packets[window_size];// = new L2Packet[window_size];
		for (unsigned int i = 0; i < window_size; i++)
		{
			send_packets[i] = new L2Packet(length);
		}
		std::vector<std::pair<Counter, unsigned int> > timeout_tracker;

		// Reset timeout for initial packets
		for (unsigned int i = 0; i < timeout_tracker.size(); i++)
		{
			timeout_tracker[i].first.reset(timeout);
		}

		while (filestr.good())
		{
			// Cache batch of packets to send
			for (unsigned int i = 0; i < window_size && filestr.good(); i++)
			{
				if (debug)
					printf("Beginning to read from file.\n");

				L2Packet* send_packet = send_packets[i];

				send_packet->clear();

				// establish values for sender's L2 packet
				send_packet->type() = 'D';
				send_packet->seq() = seq_no;
				send_packet->priority() = priority;
				send_packet->src_ip_addr() = curr_addr.sin_addr.s_addr;
				send_packet->src_port() = curr_addr.sin_port;
				send_packet->dest_ip_addr() = requester_addr.sin_addr.s_addr;
				send_packet->dest_port() = requester_addr.sin_port;

				filestr.read(send_packet->payload(), length);
				send_packet->length() = (unsigned int) filestr.gcount();
				send_packet->l1_length() = send_packet->length() + L1_HEADER;

				if (debug)
				{
					printf("Packet cached:\n");
					send_packet->print();
					printf("Index: %d\n", i);
					printf("Destination: %s %u\n\n",
						   inet_ntoa(requester_addr.sin_addr),
						   ntohs(requester_addr.sin_port));
				}

				// Create entry if data is left to be sent
				if (send_packet->length() !=0)
					timeout_tracker.push_back(std::pair<Counter, unsigned int>(Counter(rate), i));

				++seq_no;
			}

			// Check for ack packets and send as necessary
			while (!timeout_tracker.empty())
			{
				fflush(stdout);

				// send batch
				for (unsigned int i = 0; i < timeout_tracker.size(); i++)
				{
					// Resend if timeout expired
					if (timeout_tracker[i].first.check())
					{
						unsigned int key = timeout_tracker[i].second;
						L2Packet* send_packet = send_packets[key];

						// Send packets
						if (debug)
						{
							printf("Packet being sent:\n");
							send_packet->print();
							printf("Index: %d\n", key);
							printf("Destination: %s %u\n\n",
								   inet_ntoa(requester_addr.sin_addr),
								   ntohs(requester_addr.sin_port));
						}

						sendto(sock, *send_packet, send_packet->l2_length(), 0,
								(struct sockaddr *) &emu_addr, sizeof(struct sockaddr));

						counter.wait();
					}
				}

				// ack receive
				bytes_read = recvfrom(sock, recv_packet, recv_packet.l2_length(), MSG_DONTWAIT,
					(struct sockaddr *) &requester_addr, &addr_len);

				// If ack packet was received
				if (bytes_read > 0 && recv_packet.type() == 'A')
				{
					if (debug)
					{
						printf("Packet being received:\n");
						recv_packet.print();
						printf("Bytes read: %d\n", bytes_read);
						printf("Origin: %s %u\n\n",
							   inet_ntoa(requester_addr.sin_addr),
							   ntohs(requester_addr.sin_port));
					}

					// Remove matching entry from tracker, ignore irrelevant ack packets
					for (unsigned int i = 0; i < timeout_tracker.size(); i++)
					{
						unsigned int key = timeout_tracker[i].second;
						L2Packet* send_packet = send_packets[key];

						if (send_packet->seq() == recv_packet.seq())
						{
							// Remove element from array
							timeout_tracker.erase(timeout_tracker.begin() + i);
						}

						// Break to be safe - breaking the iterator
						break;
					}

					// Extra ACK packets are ignored
				}
			}
		}
		filestr.close();

		L2Packet send_packet(0);
		send_packet.type() = 'E';
		send_packet.seq() = seq_no;
		send_packet.priority() = priority;
		send_packet.src_ip_addr() = curr_addr.sin_addr.s_addr;
		send_packet.src_port() = curr_addr.sin_port;
		send_packet.dest_ip_addr() = requester_addr.sin_addr.s_addr;
		send_packet.dest_port() = requester_addr.sin_port;

		if (debug)
		{
			printf("End packet being sent:\n");
			send_packet.print();
		}

		sendto(sock, send_packet, L1_HEADER + L2_HEADER, 0,
				(struct sockaddr *)&emu_addr, sizeof(struct sockaddr));
	}
	else
	{
		// Drop packet
	}
}
