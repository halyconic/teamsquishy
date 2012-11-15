/*
 * main.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: KevStev
 *
 *      TODO: src addr
 */

#include <unistd.h> //getopt
#include <ctype.h>  //isprint
#include <stdlib.h> //exit
#include <stdio.h>
#include <ctime>

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
#include <vectors>

#include "tracker.h"
#include "packet.h"
#include <algorithm>
#include <sys/time.h>
#include <fstream>
#include "time.h"

const char* domain = ".cs.wisc.edu";

struct Super_Packet
{
	L2Packet *packet;
	timeval time;

	//time stamp
	Super_Packet(L2Packet *p, timeval t)
	{
		packet = p;
		time = t;
	}

	void print()
	{
		packet->print();

		// time
		printf("seconds: %d\n", time.tv_sec);
	}
};

bool compare (Super_Packet p1, Super_Packet p2)
{
	if (p1.packet->seq() < p2.packet->seq())
		return true;
	else
		return false;
}

void printTime()
{
	//print out the current time
	time_t now;
	struct tm *tm;

	now = time(0);
	if ((tm = localtime (&now)) == NULL)
	{
		printf ("Error extracting time stuff\n");
		return ;
	}

	printf ("%04d-%02d-%02d %02d:%02d:%02d\n",
		tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
}

double diff_us(timeval t1, timeval t2)
{
    return (double) (((t1.tv_sec - t2.tv_sec) * 1000000) +
            (t1.tv_usec - t2.tv_usec));
}

double diff_ms(timeval t1, timeval t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) +
            (t1.tv_usec - t2.tv_usec))/1000.0;
}

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
	char* arg_file_option = NULL;		//o
	char* arg_emu_hostname = NULL;		//f
	char* arg_emu_port = NULL;			//h
	char* arg_window = NULL;			//w

	// Our personal debug options
	bool debug = false;					//d
	char* arg_debug = NULL;				//d

	while ((cmd = getopt(argc, argv, "p:o:f:h:w:d:")) != -1)
	{
		switch (cmd)
		{
		case 'p':
			arg_port = optarg;
			break;
		case 'o':
			arg_file_option = optarg;
			break;
		case 'f':
			arg_emu_hostname = optarg;
			break;
		case 'h':
			arg_emu_port = optarg;
			break;
		case 'w':
			arg_window = optarg;
			break;
		case 'd':
			debug = true;
			arg_debug = optarg;
			break;
		case '?':
			if (optopt == 'p' || optopt == 'o' ||
				optopt == 'f' || optopt == 'h' || optopt == 'w' ||
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
	else if (!arg_file_option)
	{
		printf("Please supply a file option (Usage: -o <file_option>).\n");
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
	else if (!arg_window)
	{
		printf("Please supply a packet window (Usage: -w <window>).\n");
		return 0;
	}

	/*
	 *  Convert arguments to usable form
	 */

	unsigned long int port = strtoul(arg_port, NULL, 0);
	unsigned long int emu_port = strtoul(arg_emu_port, NULL, 0);
	unsigned long int window_size = strtoul(arg_window, NULL, 0);

	// Aliases
	char* file_option = arg_file_option;
	char* emu_hostname = arg_emu_hostname;

	/*
	 * Verify variables are within the correct range
	 *
	 * TODO: test new params
	 */

	if (port < 1024 || port > 65536
			|| emu_port < 1024 || emu_port > 65536)
	{
		printf("Please supply a port number between 1025 and 65535.\n");
		return 0;
	}

	/*
	 * Parse tracker.txt
	 */

	std::vector<TrackerEntry> tracker = get_tracker_from_file("tracker.txt", debug);

	if (0 && debug)
	{
		printf("Output entries:\n");
		for (unsigned int i = 0; i < tracker.size(); i++)
		{
			printf("%s", tracker[i].filename);
			printf(" ");
			printf("%d", tracker[i].id);
			printf(" ");
			printf("%s", tracker[i].machinename);
			printf(" ");
			printf("%d", tracker[i].port);
			printf("\n");
		}
	}

	// Set up socket connection
	int send_sock, recv_sock;
	struct sockaddr_in requester_addr, sender_addr, emu_addr;
	struct hostent *emu_ent, *dest_ent;
	int bytes_read;
	socklen_t addr_len;

	if ((send_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	/*
	 * Set up send
	 */

	// Where we are sending to
	sender_addr.sin_family = AF_INET;
	addr_len = sizeof(struct sockaddr);

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

	if (debug)
	{
		printf("IP emulator lookup: %s\n", ip_lookup);
	    printf("Next hop: %s %u\n",
			   inet_ntoa(emu_addr.sin_addr),
			   ntohs(emu_addr.sin_port));
	}

	/*
	 * Set up receive
	 */

	if (0 && debug)
	{
		printf("The port is %lu\n", port);
		fflush(stdout);
	}

	// Own address
	requester_addr.sin_family = AF_INET;
	requester_addr.sin_port = htons(port);
	requester_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(requester_addr.sin_zero), 8);

	if ((recv_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	// Bind port to listen on
	if (bind(recv_sock, (struct sockaddr *) &requester_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Bind");
		exit(1);
	}

	/*
	 * Make requests
	 */

	unsigned int num_active_senders = 0;
	unsigned int number_of_parts = 0;
	for (unsigned int i = 0; i < tracker.size(); i++)
	{
		if (strcmp(file_option, tracker[i].filename) == 0)
		{
			num_active_senders++;

			if (0 && debug)
			{
				printf("Entry acknowledged:\n");
				printf("%s %d %s %d\n",
					tracker[i].filename,
					tracker[i].id,
					tracker[i].machinename,
					tracker[i].port);
			}

			/*
			 * Send request
			 */

			// Adjust our destination
			char* ip_lookup_dest = new char[strlen(tracker[i].machinename) + strlen(domain)];
			ip_lookup_dest = strcat(tracker[i].machinename, domain);
			dest_ent = (struct hostent *) gethostbyname(ip_lookup_dest);

			if (debug)
			{
				printf("IP dest lookup: %s\n", ip_lookup_dest);
			}

			// Verify sender exists
			if ((struct hostent *) dest_ent == NULL)
			{
				// TODO: Gracefully handle missing sender
				printf("Host was not found by the name of %s\n", ip_lookup_dest);
				exit(1);
			}

			// Set up destination address (depreciated)
			sender_addr.sin_port = htons(tracker[i].port);
			sender_addr.sin_addr = *((struct in_addr *)dest_ent->h_addr);
			bzero(&(sender_addr.sin_zero), 8);

			// Form packet
			L2Packet send_packet = L2Packet(strlen(file_option));
			send_packet.type() = 'R';
			send_packet.seq() = 0;
			send_packet.length() = window_size;
			send_packet.priority() = 1;
			send_packet.src_ip_addr() = requester_addr.sin_addr.s_addr;
			send_packet.src_port() = requester_addr.sin_port;
			send_packet.dest_ip_addr() = sender_addr.sin_addr.s_addr;
			send_packet.dest_port() = sender_addr.sin_port;
			send_packet.l1_length() = L1_HEADER;
			strcpy(send_packet.payload(), file_option);

			if (debug)
			{
				printf("Packet being sent:\n");
				send_packet.print();
			    printf("Destination: %s %u\n",
					   inet_ntoa(sender_addr.sin_addr),
					   ntohs(sender_addr.sin_port));
			}

			sendto(send_sock, send_packet, send_packet.l2_length(), 0,
					(struct sockaddr *) &emu_addr, sizeof(struct sockaddr));

			if (debug)
				printf("Packet sent\n");

			number_of_parts++;
		}
	}

	if (number_of_parts <= 0)
	{
		printf("Tracker file does not contain any files by the name %s.\n", file_option);
		return(0);
	}

	/*
	 * Listen for packets (Listen until end packet)
	 */

	if (debug)
	{
		printf("Requester waiting for sender on port %lu\n", port);
		fflush(stdout);
	}

	// Initialize immutable components of ack packet
	L2Packet ack_packet = L2Packet(0);
	ack_packet.type() = 'A';
	ack_packet.length() = 0;
	ack_packet.priority() = 1;
	ack_packet.l1_length() = L1_HEADER;

	std::vector<Super_Packet> packets_list;
	timeval begin_time;
	gettimeofday(&begin_time, NULL);
	while (num_active_senders > 0)
	{
		if (debug)
			fflush(stdout);

		L2Packet* recv_packet = new L2Packet();

		bytes_read = recvfrom(recv_sock, *recv_packet, recv_packet->l2_length(), 0,
			(struct sockaddr *) &emu_addr, &addr_len);

		// get time stamp of current packet
		struct timeval curr_time;
		gettimeofday(&curr_time, NULL);

		if (debug)
		{
			printf("Packet received:\n");
			recv_packet->print();
		}

		if (recv_packet->type() == 'D')
		{
			if (debug)
			{
				recv_packet->print();
				printTime();
				printf("Destination: %s %u\n",
					   inet_ntoa(sender_addr.sin_addr),
					   ntohs(sender_addr.sin_port));
				printf("added packet to list with sequence number: %d\n\n", recv_packet->seq());
			}

			// add packet to vector
			packets_list.push_back(Super_Packet(recv_packet, curr_time));

			/*
			 * Send acknowledgement
			 */

			ack_packet.seq() = recv_packet->seq();
			ack_packet.dest_ip_addr() = recv_packet->src_ip_addr();
			ack_packet.dest_port() = recv_packet->src_port();
			ack_packet.src_ip_addr() = requester_addr.sin_addr.s_addr;
			ack_packet.src_port() = requester_addr.sin_port;

			if (debug)
			{
				printf("Ack sent to destination: %s %u\n\n",
					   inet_ntoa(sender_addr.sin_addr),
					   ntohs(sender_addr.sin_port));
			}

			sendto(send_sock, ack_packet, ack_packet.l2_length(), 0,
					(struct sockaddr *) &emu_addr, sizeof(struct sockaddr));
		}
		else if (recv_packet->type() == 'E')
		{
			if (debug)
			{
				recv_packet->print();
				printTime();
				printf("Destination: %s %u\n",
					   inet_ntoa(sender_addr.sin_addr),
					   ntohs(sender_addr.sin_port));
				printf("added packet to list with sequence number: %d\n\n", recv_packet->seq());
			}

			// add packet to vector
			packets_list.push_back(Super_Packet(recv_packet, curr_time));
			num_active_senders--;
		}
		else
		{
			if (debug)
			{
				printf("Unexpected packet received:\n");
				recv_packet->print();
				printf("Destination: %s %u\n",
					   inet_ntoa(sender_addr.sin_addr),
					   ntohs(sender_addr.sin_port));
				printf("Unexpected packet was dropped\n\n");
				// Print packet contents
			}
		}
	}

	// sort packets by sequence number :)
	std::sort (packets_list.begin(), packets_list.end(), compare);

	if (debug)
	{
		printf("\nDEBUG\n\n");

		for (unsigned int i = 0; i < packets_list.size(); i++)
		{
			packets_list.at(i).print();
			printf("\n");
		}
	}

	printf("\nBEGIN SUMMARY\n\n");

	// SUMMARY
	// total data packets received
	printf("Total data packets received: %d\n", packets_list.size());

	//total data bytes received
	int sum_of_bytes;
	for (unsigned int i = 0; i < packets_list.size(); i++)
	{
		sum_of_bytes += packets_list.at(i).packet->length();
	}
	printf("total data bytes received: %d\n", sum_of_bytes);

	// average packets/second (not working right now)

	int senderNumber = 1;
	int numPacketsAtSender = 0.0;
	float diff_time;
	double avg_diff_ms;
	double avg_diff_us;
	Super_Packet next_packet = packets_list.at(0);
	for (int i = 0; i < packets_list.size(); i++)
	{
		if (packets_list.at(i).packet->type() == 'D')
		{
			numPacketsAtSender++;
		}
		else if (packets_list.at(i).packet->type() == 'E')
		{
			numPacketsAtSender++;

			diff_time = packets_list.at(i).time.tv_sec - begin_time.tv_sec;
			//diff_time = diff_ms(packets_list.at(i).time, begin_time);


			avg_diff_ms = ((double)diff_ms(packets_list.at(i).time, begin_time))/numPacketsAtSender;
			avg_diff_us = ((double)diff_us(packets_list.at(i).time, begin_time))/numPacketsAtSender;
			printf("average milliseconds for sender %d: %f\n", senderNumber, avg_diff_ms);
			printf("average microseconds for sender %d: %f\n", senderNumber, avg_diff_us);

			senderNumber++;
			numPacketsAtSender = 0;
			//next_packet = packets_list.at(i+1);

		}
	}

	// duration of the test
	double diff_time_ms = (double)diff_ms(packets_list.back().time, begin_time);
	double diff_time_us = (double)diff_us(packets_list.back().time, begin_time);

	printf("duration of entire test: %f milliseconds\n", diff_time_ms);
	printf("duration of entire test: %f microseconds\n", diff_time_us);

	time_t now;
	struct tm *tm;

	now = time(0);
	if ((tm = localtime (&now)) == NULL)
	{
		printf ("Error extracting time stuff\n");
	}

	printf ("%04d-%02d-%02d %02d:%02d:%02d\n",
			tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);

	/*
	 * Print to file
	 */

	unsigned int last_seq_no = 0;
	std::ofstream myfile;
	myfile.open (file_option);
	for (unsigned int i = 0; i < packets_list.size(); i++)
	{
		if (last_seq_no != packets_list.at(i).packet->seq())
			myfile << packets_list.at(i).packet->payload();

		last_seq_no = packets_list.at(i).packet->seq();
	}
	myfile.close();
}

