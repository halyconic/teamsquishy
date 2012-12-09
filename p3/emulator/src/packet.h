/*
 * packet.h
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#ifndef PACKET_H_
#define PACKET_H_

const int HEADER_LENGTH = 29;

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

	void print()
	{
		//
	}

	void clear()
	{
		bzero(values_, 29);
	}

	Packet()
	{
		values_ = new char[29];
	}

	~Packet()
	{
		delete values_;
	}

private:
	char* values_;
};

#endif /* PACKET_H_ */
