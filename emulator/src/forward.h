/*
 * forward.h
 *
 *  Created on: Nov 11, 2012
 *      Author: swebber
 */

#ifndef FORWARD_H_
#define FORWARD_H_

#include <vector>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>

// IP addresses are stored in reverse order
class ForwardEntry
{
public:

	unsigned int emu_ip; //char* emu_hostname;
	unsigned short int emu_port;
	unsigned int dest_ip; //char* dest_hostname;
	unsigned short int dest_port;
	unsigned int next_ip; //char* next_hostname;
	unsigned short int next_port;
	unsigned int delay; // milliseconds
	unsigned char loss; // 0 to 100 percent

	ForwardEntry() :
		emu_ip(0),
		emu_port(0),
		dest_ip(0),
		dest_port(0),
		next_ip(0),
		next_port(0),
		delay(0),
		loss(0) {;}

	ForwardEntry(
			char* e_h, unsigned short int e_p,
			char* d_h, unsigned short int d_p,
			char* n_h, unsigned short int n_p,
			unsigned int d, unsigned char l);

	void print()
	{
		printf("%o %d %o %d %o %d %d %d\n",
				emu_ip,
				htons(emu_port),
				dest_ip,
				htons(dest_port),
				next_ip,
				htons(next_port),
				delay,
				loss);
	}
};

/*
 * Return entries from file
 */
std::vector<ForwardEntry> get_forwarding_table_from_file(char* filename, bool debug);

#endif /* FORWARD_H_ */
