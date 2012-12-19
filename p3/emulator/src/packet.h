/*
 * packet.h
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#ifndef PACKET_H_
#define PACKET_H_

#include "utils.h"

const int HEADER_LENGTH = 29;
const int ROUTE_LENGTH = 21*4;

struct Packet
{
	char& type()
	{
		return values_[0];
	}
	unsigned long int& TTL()
	{
		return (unsigned long int&)values_[1];
	}
	unsigned long int& src_ip_addr()
	{
		return (unsigned long int&)values_[9];
	}
	unsigned short int& src_port()
	{
		return (unsigned short int&)values_[17];
	}
	unsigned long int& dest_ip_addr()
	{
		return (unsigned long int&)values_[19];
	}
	unsigned short int& dest_port()
	{
		return (unsigned short int&)values_[27];
	}

	char  operator [] (unsigned i) const { return this->values_[i]; }
	char& operator [] (unsigned i)       { return this->values_[i]; }
	operator char*()                     { return this->values_; }

	void set_source(Address source)
	{
		src_ip_addr() = source.first;
		src_port() = source.second;
	}

	Address get_source()
	{
		Address source = Address(src_ip_addr(), src_port());
		return source;
	}

	void set_destination(Address destination)
	{
		dest_ip_addr() = destination.first;
		dest_port() = destination.second;
	}

	Address get_destination()
	{
		Address destination = Address(dest_ip_addr(), dest_port());
		return destination;
	}

	void print()
	{
		printf("type: %c, source: %lu, %d dest: %lu, %d, TTL: %lu\n", type(), TTL(), src_ip_addr(), src_port(), dest_ip_addr(), dest_port());
//		printf("(%s, %d) -> (%s, %d)\n",
//				inet_ntoa(*((struct in_addr *)src_ip_addr())), ntohs(src_port()),
//				inet_ntoa(*((struct in_addr *)dest_ip_addr())), ntohs(dest_port()));
	}

	void clear()
	{
		bzero(values_, HEADER_LENGTH);
	}

	Packet()
	{
		values_ = new char[HEADER_LENGTH];
	}

	~Packet()
	{
		delete values_;
	}

private:
	char* values_;
};

struct RoutePacket : Packet
{
	unsigned int& sequence_number()
	{
		return (unsigned int&)values_[HEADER_LENGTH];
	}

	char* route_array()
	{
		return &values_[HEADER_LENGTH + 4];
	}

	void print()
	{
		if (type() == 'R')
		{
<<<<<<< HEAD
			printf("type: %c, source: %lu, %d dest: %lu, %d, TTL: %lu, Seq no: %d\n", type(), TTL(), src_ip_addr(), src_port(), dest_ip_addr(), dest_port(), sequence_number());
//			printf("(%s, %d) -> (%s, %d)\n",
//					inet_ntoa(*((struct in_addr *)src_ip_addr())), ntohs(src_port()),
//					inet_ntoa(*((struct in_addr *)dest_ip_addr())), ntohs(dest_port()));
=======
			printf("type: %c, TTL: %lu source: %lu, %d dest: %lu, %d sequence no: %d\n", type(), TTL(), src_ip_addr(), src_port(), dest_ip_addr(), dest_port(), sequence_number());
			printf("(%s, %d) -> (%s, %d)\n",
					inet_ntoa(*((struct in_addr *)src_ip_addr())), ntohs(src_port()),
					inet_ntoa(*((struct in_addr *)dest_ip_addr())), ntohs(dest_port()));
>>>>>>> 42467004f0799de53df57fb2f9aabe0a88791cde
		}
		else
		{
			Packet::print();
		}
	}

	void clear()
	{
		bzero(values_, HEADER_LENGTH + ROUTE_LENGTH);
	}

	RoutePacket()
	{
		values_ = new char[HEADER_LENGTH + ROUTE_LENGTH];
	}

	~RoutePacket()
	{
		delete values_;
	}

private:
	char* values_;
};

#endif /* PACKET_H_ */
