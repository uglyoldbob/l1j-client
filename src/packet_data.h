#ifndef __PACKET_DATA_H__
#define __PACKET_DATA_H__

#include <stdint.h>
#include <vector>

class packet_data
{
	public:
		packet_data();
		packet_data &operator = (std::vector<unsigned char> a);
		packet_data &operator << (const char *a);
		packet_data &operator << (char *a);
		packet_data &operator << (uint8_t a);
		packet_data &operator << (uint16_t a);
		packet_data &operator << (uint32_t a);
		packet_data &operator >> (uint8_t &a);
		packet_data &operator >> (int8_t &a);
		packet_data &operator >> (uint16_t &a);
		packet_data &operator >> (int16_t &a);
		packet_data &operator >> (uint32_t &a);
		packet_data &operator >> (int32_t &a);
		packet_data &operator >> (char *&a);
		packet_data &operator << (std::vector<unsigned char> a);
		void insert(uint16_t me);
		void insert(uint8_t me);
		unsigned char &operator [] (int a);
		void clear();
		int size();
	private:
		std::vector<unsigned char> data;
};

#endif
