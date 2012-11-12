#include <fstream>
#include <stdio.h>

const int DEFAULT_PAYLOAD = 5*1024;
const int L1_HEADER = 1 + 4 + 4;
const int L2_HEADER = 1 + 4 + 2 + 4 + 2 + 4;

struct L1Packet
{
	int payload_buffer_size;

	/*
	 * L1 fields
	 */

	char& type()
	{
		return values_[0];
	}
	unsigned int& seq()
	{
		return (unsigned int&)values_[1];
	}
	unsigned int& length()
	{
		return (unsigned int&)values_[5];
	}
	char* payload()
	{
		return &values_[L1_HEADER];
	}

	char  operator [] (unsigned i) const { return this->values_[i]; }
	char& operator [] (unsigned i)       { return this->values_[i]; }
	operator char*()                     { return this->values_; }

	// TODO: This does not work if payload is full!!!
	void print()
	{
		printf("%c %d %d\n%s\n",
			   type(),
			   seq(),
			   length(),
			   payload());
	}

	// Assumes length < payload
	void clear(unsigned int length)
	{
		bzero(values_, length);
	}

	// Create a packet of arbitrary length (default payload is 5KB)
	L1Packet(unsigned int payload = DEFAULT_PAYLOAD)
	{
		values_ = new char[L1_HEADER + payload];
		payload_buffer_size = payload;
	}

	~L1Packet()
	{
		delete values_;
	}

private:
	char* values_;
};

struct L2Packet
{
	int payload_buffer_size;

	/*
	 * L2 fields
	 */

	char& priority()
	{
		return values_[0];
	}
	unsigned int& src_ip_addr()
	{
		return (unsigned int&)values_[1];
	}
	unsigned short int& src_port()
	{
		return (unsigned short int&)values_[5];
	}
	unsigned int& dest_ip_addr()
	{
		return (unsigned int&)values_[7];
	}
	unsigned short int& dest_port()
	{
		return (unsigned short int&)values_[11];
	}
	unsigned int& l1_length()
	{
		return (unsigned int&)values_[13];
	}

	/*
	 * L1 fields
	 */

	char& type()
	{
		return values_[L2_HEADER + 0];
	}
	unsigned int& seq()
	{
		return (unsigned int&)values_[L2_HEADER + 1];
	}
	unsigned int& length()
	{
		return (unsigned int&)values_[L2_HEADER + 5];
	}
	char* payload()
	{
		return &values_[L2_HEADER + L1_HEADER];
	}

	char  operator [] (unsigned i) const { return this->values_[i]; }
	char& operator [] (unsigned i)       { return this->values_[i]; }
	operator char*()                     { return this->values_; }

	// TODO: This does not work if payload is full!!!
	void print()
	{
		printf("%c %d %d\n%s\n",
			   type(),
			   seq(),
			   length(),
			   payload());
	}

	// Assumes length < payload
	void clear(unsigned int length)
	{
		bzero(values_, length);
	}

	// Create a packet of arbitrary length (default payload is 5KB)
	L2Packet(unsigned int payload = DEFAULT_PAYLOAD)
	{
		values_ = new char[L2_HEADER + L1_HEADER + payload];
		payload_buffer_size = payload;
	}

	~L2Packet()
	{
		delete values_;
	}

private:
	char* values_;
};
