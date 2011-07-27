#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "packet.h"
#include "unsorted.h"

static const int MAX_LENGTH = 0x13fe;

char packet::decryptionKey[8];
char packet::encryptionKey[8];
int packet::mode = 0;
int packet::key_initialized = 0;

int packet::assemble(char *buf, int max_length, const char *format, ...)
{
	int length;
	va_list temp_args;
	va_start(temp_args, format);
	length = assemble(buf, max_length, format, temp_args);
	va_end(temp_args);
	return length;
}

const unsigned char* packet::disassemble(unsigned char *buf, const char *format, ...)
{
	int i = 0;
	int buf_offset = 0;
	va_list args;
	va_start(args, format);
	while (format[i] != 0)
	{
		switch(format[i])
		{
			case 'c':
			{
				unsigned char temp = buf[buf_offset++];
				unsigned char *store = va_arg(args, unsigned char *);
				*store = temp;
				break;
			}
			case 'd':
			{
				unsigned long *from = (unsigned long*)(&buf[buf_offset]);
				buf_offset += 4;
				unsigned long *store = va_arg(args, unsigned long *);
				*store = *from;
				break;
			}
			default:
				break;
		}
		i++;
	}
	va_end(args);
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
		format_offset++;
	}
	return send_offset;
}

void packet::sendPacket(const char* args, ...)
{
	printf("Sending a packet %s\n", args);
	char sendbuf[MAX_LENGTH];
	va_list temp_args;
	va_start(temp_args, args);
	int length;
	
	length = assemble(&sendbuf[2], MAX_LENGTH, args, temp_args);
	va_end(temp_args);
	if (length != 0)
	{
		sendbuf[0] = length & 0xff;
		sendbuf[1] = (length>>8) & 0xff;
		packet_length = length;
		packet_data = new unsigned char[packet_length];
		memcpy(packet_data, sendbuf, packet_length);
		this->encrypt();
		this->change_key(encryptionKey, &sendbuf[2]);
		server->snd(packet_data, length+2);
	}
}

void packet::getPacket(const char* args, ...)
{
	int length = 0;
	server->rcv(&length, 2);
	printf("Packet length %08x\n", length);
	if (length != 0)
	{
		if (packet_length == 0)
		{
			packet_length = length;
			packet_data = new unsigned char[packet_length-2];
			printf("Receiving data\n");
			server->rcv(packet_data, packet_length-2);
			if (key_initialized == 1)
			{
				this->decrypt();
				this->change_key(decryptionKey, (char*)(packet_data));
			}
			else
			{
				printf("Not decrypting this packet\n");
			}
			//determine if it should be decrypted
			//process the packet
		}
		else
		{
			printf("ERROR: trying to load 2 packets\n");
		}
	}
}

void packet::create_key(const unsigned long seed)
{
	unsigned long key = 0x930FD7E2;
	unsigned long big_key[2];
	big_key[0] = seed;
	big_key[1] = key;
	
	unsigned long rotrParam = big_key[0] ^ 0x9C30D539;
	big_key[0] = (rotrParam>>13) | (rotrParam<<19);	//rotate right by 13 bits
	big_key[1] = big_key[0] ^ big_key[1] ^ 0x7C72E993;
	
	//WARNING: possibly backwards here	
	memcpy(encryptionKey, big_key, 8);
	memcpy(decryptionKey, big_key, 8);
	
	key_initialized = 1;
}

void packet::change_key(char *key, const char* data)
{	//decryptionKey, &buffer[2]
	key[0] ^= data[0];
	key[1] ^= data[1];
	key[2] ^= data[2];
	key[3] ^= data[3];
	
	unsigned long *temp = (unsigned long*)(&key[4]);
	*temp += 0x287EFFC3;
}

void packet::encrypt()
{
	if (packet_data != 0)
	{
		printf("Encrypting data\n");
		packet_data[2] ^= encryptionKey[0];

		for (int i = 3; i < packet_length; i++)
		{
			packet_data[i] ^= encryptionKey[i & 7] ^ packet_data[i-1];
		}

		packet_data[5] ^= encryptionKey[2];
		packet_data[4] ^= encryptionKey[3] ^ packet_data[5];
		packet_data[3] ^= encryptionKey[4] ^ packet_data[4];
		packet_data[2] ^= encryptionKey[5] ^ packet_data[3];
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
	packet_data = (unsigned char*)0;
	if (key_initialized == 0)
	{
		printf("Retrieving encryption key from server\n");
		getPacket("asdf", 1);
		if (packet_data[0] == 0x41)
		{
			processPacket();
			getPacket("asdf", 1);
			processPacket();
			
		}
		else
		{
			printf("This server is not compatible with this client\n");
		}
	}
}

void packet::reset()
{
	if (packet_length != 0)
	{
		delete [] packet_data;
		packet_length = 0;
	}
}

void packet::processPacket()
{
	if (mode == 0)
	{
		switch(packet_data[0])
		{	//the second list
			case 0x0a:
			{
				serverVersionPacket();
				break;
			}
			case 0x41:
			{	//receive encryption key data
				keyPacket();
				break;
			}
			default:
				//b0648
				break;
		}
	}
	else
	{
		switch(packet_data[0])
		{
			default:
				break;
		}
	}
}

packet::~packet()
{
	reset();
}

void packet::keyPacket()
{
	unsigned long seed = packet_data[4]<<24 |
		packet_data[3]<<16 |
		packet_data[2]<<8 |
		packet_data[1];
	create_key(seed);
	reset();
	//loadFile("VersionInfo", &r1_40);
	sendPacket("chdcd", 0x47, 0x33, -1, 32, 101101);
		//acp, VersionInfo, buildNumber
		//-1, 32, 101101
	reset();
}

void packet::serverVersionPacket()
{
	char version_check;
	unsigned long serverVersion;
	unsigned long cacheVersion;
	unsigned long authVersion;
	unsigned long npcVersion;
	unsigned long serverStartTime;
	char canMakeNewAccount;
	char englishOnly;
	char countryCode;
	char serverCode;
	disassemble(&packet_data[1], "c", &version_check);
	if (version_check == 1)
	{
		printf("ERROR: Load version data from somewhere else\n");
	}
	else
	{
		disassemble(&packet_data[2], "cdddddcc", &serverCode, &serverVersion, 
			&cacheVersion, &authVersion, &npcVersion, &serverStartTime,
			&canMakeNewAccount, &englishOnly);
	}
	disassemble(&packet_data[25], "c", &countryCode);
	
	unsigned short serverId = (countryCode<<8) + serverCode;
	printf("Server id: %04x\n", serverId);
	if (serverId == 0x012c)
	{
		LoadDurationTable("spelldur300.tbl");
	}
	else
	{
		LoadDurationTable("spelldur.tbl");
	}
	if (countryCode == 3)
	{
		init_codepage(0x3b6);	//memset(lead_table, 0, 0x100); memset(lower_table, 0, 0x100); 
//		serverCP = 0x3b6;
	}
	else if (countryCode == 4)
	{
		init_codepage(0x3a4);
//		serverCP = 0x3a4;
	}
	else if (countryCode == 0)
	{
		init_codepage(0x3b5);
//		serverCP = 0;
	}
	else if (countryCode == 5)
	{
		init_codepage(0x3a8);
//		serverCP = 0x3a8;
	}
	else
	{
		init_codepage(0x4e4);
//		serverCP = 0;
	}
	printf("STUB AdjustExp()\n");
//	AdjustExp();
	printf("STUB Modify serverCP and acp\n");
//	useOtherLang = 0;
//	if (serverCP == 0x3a4)
//	{
//		if (clientCP == 0x3a4)
//		{
//			useOtherLang = 1;
//
//			acp = serverCP;
//		}
//		else
//		{
//			endLoading(1);
//			if (strs <= 0x22e)
//			{
//				messageBox();
//			}
//			else
//			{
//				r2 = strs[4];
//				r3 = strs[4][0x8b8];
//				messageBox();
//			}
//			PostQuitMessage(0);
//			return;
//		}
//
//	}
//	else if (serverCP == 0x3b6)
//	{
//		if ((clientCP != 0x3b6) && (clientCP != 0x4e4))
//		{
//			acp = serverCP;
//		}
//	}
//	else if (serverCP == 0x3a8)
//	{
//		if (clientCP == 0x3a8)
//		{
//			acp = clientCP;
//		}
//		else
//		{
//			endLoading(1);
//			if (strs <= 0x22e)
//			{
//				messageBox();
//			}
//			else
//			{
//				r2 = strs[4];
//				r3 = strs[4][0x8b8];
//				messageBox();
//			}
//			PostQuitMessage(0);
//			return;
//		}
//	}
//
//	if (countryCode == 1)
//	{
//		changeNewAccountInfo();
//	}
	printf("STUB LoadEmblemFile()\n");
//	LoadEmblemFile(serverId);
	return;
}

