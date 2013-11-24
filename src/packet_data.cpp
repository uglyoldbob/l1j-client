#include "packet_data.h"

#include <stdio.h>
#include <string.h>

packet_data::packet_data()
{
}

packet_data &packet_data::operator << (uint8_t a)
{
	data.push_back(a);
	return *this;
}

void packet_data::insert(uint8_t me)
{
	data.insert(data.begin(), me);
}

packet_data &packet_data::operator >> (uint8_t &a)
{
	a = data[0] & 0xFF;
	data.erase(data.begin());
	return *this;
}

packet_data &packet_data::operator >> (int8_t &a)
{
	a = data[0] & 0xFF;
	data.erase(data.begin());
	return *this;
}

void packet_data::insert(uint16_t me)
{
	data.insert(data.begin(), (me>>8) & 0xFF);
	data.insert(data.begin(), me & 0xFF);
}

packet_data &packet_data::operator >> (uint16_t &a)
{
	a = data[0] & 0xFF;
	a |= ((data[1] << 8) & 0xFF00);
	data.erase(data.begin(), data.begin()+2);
	return *this;
}

packet_data &packet_data::operator >> (int16_t &a)
{
	a = data[0] & 0xFF;
	a |= ((data[1] << 8) & 0xFF00);
	data.erase(data.begin(), data.begin()+2);
	return *this;
}

packet_data &packet_data::operator << (uint16_t a)
{
	data.push_back(a & 0xFF);
	data.push_back((a>>8) & 0xFF);
	return *this;
}

packet_data &packet_data::operator >> (uint32_t &a)
{
	a = data[0] & 0xFF;
	a |= ((data[1] << 8) & 0xFF00);
	a |= ((data[2] << 16) & 0xFF0000);
	a |= ((data[3] << 24) & 0xFF000000);
	data.erase(data.begin(), data.begin()+4);
	return *this;
}

packet_data &packet_data::operator >> (int32_t &a)
{
	a = data[0] & 0xFF;
	a |= ((data[1] << 8) & 0xFF00);
	a |= ((data[2] << 16) & 0xFF0000);
	a |= ((data[3] << 24) & 0xFF000000);
	data.erase(data.begin(), data.begin()+4);
	return *this;
}

packet_data &packet_data::operator << (uint32_t a)
{
	data.push_back(a & 0xFF);
	data.push_back((a>>8) & 0xFF);
	data.push_back((a>>16) & 0xFF);
	data.push_back((a>>24) & 0xFF);
	return *this;
}

packet_data &packet_data::operator << (std::vector<unsigned char> a)
{
	for (int i = 0; i < a.size(); i++)
	{
		data.push_back(a[i]);
	}
	return *this;
}

packet_data &packet_data::operator << (const char *&a)
{
	for (int i = 0; i <= strlen(a); i++)
	{
		data.push_back(a[i]);
	}
	return *this;
}

packet_data &packet_data::operator >> (char *&a)
{
	int i;
	for (i = 0; data[i] != 0; i++)
	{
	}
	i++;
	a = new char[i];
	for (int j = 0; j < i; j++)
	{
		a[j] = data[j];
	}
	data.erase(data.begin(), data.begin()+i);
	return *this;
}

packet_data &packet_data::operator = (std::vector<unsigned char> a)
{
	data = a;
	return *this;
}

void packet_data::clear()
{
	data.clear();
}

unsigned char &packet_data::operator [] (int a)
{
	return data[a];
}

int packet_data::size()
{
	return data.size();
}