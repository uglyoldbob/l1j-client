#include "packet.h"

#include <stdio.h>
#include <string.h>

static const int MAX_LENGTH = 0x13fe;

char packet::decryptionKey[8];
char packet::encryptionKey[8];

int packet::assemble(char *buf, int max_length, const char *format, ...)
{
	int length;
	va_list temp_args;
	va_start(temp_args, format);
	length = assemble(buf, max_length, format, temp_args);
	va_end(temp_args);
	return length;
}

int packet::assemble(char *send, int max_length, const char *format, va_list array)
{
	int format_offset = 0;
	int send_offset = 0;
	while (format[format_offset] != 0)
	{
		switch(format[format_offset])
		{
			case 'S':	//wide character string
			{
				wchar_t *temp = va_arg(array, wchar_t *);
				int src_len = (wcslen(temp) + 1) * sizeof(wchar_t);	//r31
				if ((send_offset + src_len) <= max_length)
				{
					for (int i = 0; i < src_len; i++)
					{	//WARNING: these bytes might backwards
						wchar_t *set_me = (wchar_t*)(&send[send_offset+i*2]);
						*set_me = temp[i];
					}
					send_offset += src_len;
				}
				break;
			}
			case 'b':	//multiple bytes
			{
				int num_bytes = va_arg(array, int);
				char *temp = va_arg(array, char *);
				if ((send_offset + num_bytes) <= max_length)
				{
					memcpy(&send[send_offset], temp, num_bytes);
					send_offset += num_bytes;
				}
				break;
			}
			case 'c':	//single byte (but passed as a 4 byte variable)
			{
				char temp = va_arg(array, int) & 0xFF;
				if (send_offset < max_length)
				{	//eafec
					send[send_offset] = temp;
					send_offset++;
				}
				break;
			}
			case 'd':	//a single 4-byte variable
			{
				long temp = va_arg(array, long);
				if ((send_offset + 4) <= max_length)
				{
					long *set_me = (long*)(&send[send_offset]);
					*set_me = temp;
					send_offset += 4;
				}
				break;
			}
			case 'h':	//a single 2-byte variable
			{
				short temp = va_arg(array, int) & 0xFFFF;
				if ((send_offset + 2) <= max_length)
				{
					short *set_me = (short*)(&send[send_offset]);
					*set_me = temp;
					send_offset +=2;
				}
				break;
			}
			case 's':	//a regular string
			{
				int src_len;
				char *temp = va_arg(array, char *);
				src_len = strlen(temp);
				if ((send_offset + src_len) <= max_length)
				{
					strcpy(&send[send_offset], temp);
					send_offset += (src_len + 1);
				}
				break;
			}
			default:
				break;
		}
		if (send_offset >= max_length)
			return send_offset;
	}
	return send_offset;
}

void packet::sendPacket(const char* args, ...)
{
	char *sendbuf;
	va_list temp_args;
	va_start(temp_args, args);
	int length;
			
	length = assemble(&sendbuf[2], MAX_LENGTH, args, temp_args);
	va_end(temp_args);
	if (length != 0)
	{
		sendbuf[0] = length & 0xff;
		sendbuf[1] = (length>>8) & 0xff;
		//encrypt the format before sending
		server->snd(sendbuf, length+2);
	}
}

void packet::change_key(char* data)
{
	encryptionKey[0] ^= data[0];
	encryptionKey[1] ^= data[1];
	encryptionKey[2] ^= data[2];
	encryptionKey[3] ^= data[3];
	
	unsigned long *temp = (unsigned long*)(&encryptionKey[4]);
	*temp += 0x287EFFC3;
}

void packet::encrypt()
{
	if (packet_data != 0)
	{
		packet_data[0] ^= encryptionKey[0];

		for (int i = 1; i < packet_length; i++)
		{
			packet_data[i] ^= encryptionKey[i & 7] ^ packet_data[i-1];
		}

		packet_data[3] ^= encryptionKey[2];
		packet_data[2] ^= encryptionKey[3] ^ packet_data[3];
		packet_data[1] ^= encryptionKey[4] ^ packet_data[2];
		packet_data[0] ^= encryptionKey[5] ^ packet_data[1];
	}
}

void packet::decrypt()
{
	if (packet_data != 0)
	{
		char b3 = packet_data[3];
		packet_data[3] ^= decryptionKey[2];

		char b2 = packet_data[2];
		packet_data[2] ^= (b3 ^ decryptionKey[3]);

		char b1 = packet_data[1];
		packet_data[1] ^= (b2 ^ decryptionKey[4]);

		char k = packet_data[0] ^ b1 ^ decryptionKey[5];
		packet_data[0] = k ^ decryptionKey[0];

		for (int i = 1; i < packet_length; i++) 
		{
			char t = packet_data[i];
			packet_data[i] ^= (decryptionKey[i & 7] ^ k);
			k = t;
		}
	}
}

packet::packet(connection *serve)
{
	//merely copy the existing connection so we can use it
	server = serve;
	packet_length = 0;
	packet_data = (char*)0;
}

packet::~packet()
{
}