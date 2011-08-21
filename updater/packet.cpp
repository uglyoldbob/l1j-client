#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "client.h"
#include "globals.h"
#include "packet.h"
#include "unsorted.h"

static const int MAX_LENGTH = 0x13fe;

char packet::decryptionKey[8];		//TODO : move this variable to the connection class
char packet::encryptionKey[8];		//TODO : move this variable to the connection class
int packet::mode = 0;
volatile int packet::key_initialized = 0;	//TODO : move this variable to the connection class

int packet::assemble(char *buf, int max_length, const char *format, ...)
{
	int length;
	va_list temp_args;
	va_start(temp_args, format);
	length = assemble(buf, max_length, format, temp_args);
	va_end(temp_args);
	return length;
}

void packet::disassemble(unsigned char *buf, const char *format, ...)
{
	int i = 0;
	int buf_offset = 0;
	va_list args;
	va_start(args, format);
	while (format[i] != 0)
	{
		switch(format[i])
		{
			case 's':
			{
				unsigned char length = 0;
				while (buf[buf_offset+length] != 0)
				{
					length++;
				}
				length++;
				char **temp = va_arg(args, char **);
				*temp = new char[length];
				strcpy(*temp, (char*)&buf[buf_offset]);
				buf_offset += length;
				break;
			}
			case 'h':
			{
				unsigned short *temp = (unsigned short*)&buf[buf_offset];
				unsigned short *store = va_arg(args, unsigned short *);
				*store = *temp;
				*store = SWAP16(*store);
				buf_offset += 2;
				break;
			}
			case 'c':
			{
				unsigned char temp = buf[buf_offset++];
				unsigned char *store = va_arg(args, unsigned char *);
				*store = temp;
				break;
			}
			case 'd':
			{
				unsigned int *from = (unsigned int*)(&buf[buf_offset]);
				buf_offset += 4;
				unsigned int *store = va_arg(args, unsigned int *);
				*store = SWAP32(*from);
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
				int temp = va_arg(array, int);
				if ((send_offset + 4) <= max_length)
				{
					send[send_offset+3] = temp>>24 & 0xFF;
					send[send_offset+2] = temp>>16 & 0xFF;
					send[send_offset+1] = temp>>8 & 0xFF;
					send[send_offset] = temp & 0xFF;
					send_offset += 4;
				}
				break;
			}
			case 'h':	//a single 2-byte variable
			{
				short temp = va_arg(array, int) & 0xFFFF;
				if ((send_offset + 2) <= max_length)
				{
					send[send_offset+1] = temp>>8 & 0xFF;
					send[send_offset] = temp & 0xFF;
					send_offset +=2;
				}
				break;
			}
			case 's':	//a regular string
			{
				int src_len;
				char *temp = va_arg(array, char *);
				src_len = strlen(temp) + 1;
				if ((send_offset + src_len) <= max_length)
				{
					strcpy(&send[send_offset], temp);
					send_offset += src_len;
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

void packet::send_packet(const char* args, va_list array)
{
	while (key_initialized == 0)
	{
		SDL_Delay(100);
	}
	if (packet_data == 0)
	{
		char sendbuf[MAX_LENGTH];
		int length;
		
		length = assemble(&sendbuf[2], MAX_LENGTH, args, array);
		if (length != 0)
		{
			char key_change[4];
			length += 2;
			sendbuf[0] = length & 0xff;
			sendbuf[1] = (length>>8) & 0xff;
			packet_length = length;
			packet_data = new unsigned char[packet_length];
			memcpy(packet_data, sendbuf, packet_length);
			memcpy(key_change, &sendbuf[2], 4);
			
			this->encrypt();

			this->change_key(encryptionKey, key_change);
			server->snd(packet_data, packet_length);
		}
		reset();
	}
}

void packet::send_packet(const char* args, ...)
{
	va_list temp_args;
	va_start(temp_args, args);
	send_packet(args, temp_args);
	va_end(temp_args);
}

void packet::get_packet()
{
	unsigned short length = 0;
	server->rcv_varg(&length, 2);
	if (length != 0)
	{
		if (packet_length == 0)
		{
			packet_length = length - 2;
			packet_data = new unsigned char[packet_length];
			server->rcv(packet_data, packet_length);
			if (key_initialized == 1)
			{
				this->decrypt();
				this->change_key(decryptionKey, (char*)(packet_data));
			}
		}
		else
		{
			printf("ERROR: trying to load 2 packets\n");
		}
	}
}

void packet::create_key(const unsigned int seed)
{
	unsigned int key = 0x930FD7E2;
	unsigned int big_key[2];
	big_key[0] = seed;
	big_key[1] = key;
	
	unsigned int rotrParam = big_key[0] ^ 0x9C30D539;
	big_key[0] = (rotrParam>>13) | (rotrParam<<19);	//rotate right by 13 bits
	big_key[1] = big_key[0] ^ big_key[1] ^ 0x7C72E993;
	
	encryptionKey[0] = (big_key[0]) & 0xFF;
	encryptionKey[1] = (big_key[0]>>8) & 0xFF;
	encryptionKey[2] = (big_key[0]>>16) & 0xFF;
	encryptionKey[3] = (big_key[0]>>24) & 0xFF;
	encryptionKey[4] = (big_key[1]) & 0xFF;
	encryptionKey[5] = (big_key[1]>>8) & 0xFF;
	encryptionKey[6] = (big_key[1]>>16) & 0xFF;
	encryptionKey[7] = (big_key[1]>>24) & 0xFF;

	memcpy(decryptionKey, encryptionKey, 8);

	key_initialized = 1;
}

void packet::change_key(char *key, const char* data)
{	//decryptionKey, &buffer[2]
	key[0] ^= data[0];
	key[1] ^= data[1];
	key[2] ^= data[2];
	key[3] ^= data[3];
	
	unsigned int *temp = (unsigned int*)(&key[4]);
	*temp = SWAP32(*temp);
	*temp += 0x287EFFC3;
	*temp = SWAP32(*temp);
}

void packet::encrypt()
{
	if (packet_data != 0)
	{
		packet_data[2] ^= encryptionKey[0];
		
		for (int i = 3; i < packet_length; i++)
		{
			packet_data[i] ^= encryptionKey[(i-2) & 7] ^ packet_data[i-1];
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

packet::packet(client *clnt, connection *serve)
{
	//merely copy the existing connection so we can use it
	game = clnt;
	server = serve;
	packet_length = 0;
	packet_data = (unsigned char*)0;
}

void packet::reset()
{
	if (packet_data != 0)
	{
		delete [] packet_data;
		packet_data = 0;
		packet_length = 0;
	}
}

int packet::process_packet()
{
	if (key_initialized == 0)
	{
		if (packet_data[0] != 0x41)
		{
			printf("This server is not compatible with this client\n");
			return -1;
		}
	}

	if (mode == 0)
	{
		switch(packet_data[0])
		{	//the second list
			case 10: server_version_packet(); break;
			case 21: login_check(); break;
			case 65: key_packet(); break;
			case 90: news_packet(); break;
			case 99: login_char_packet(); break;
			case 113: num_char_packet(); break;
			default: print_packet(); break;
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
	reset();
	return 0;
}

packet::~packet()
{
	reset();
}

void packet::print_packet()
{
	printf("Packet data\n\t");
	for (int i = 0; i < packet_length; i++)
	{
		printf("%02x ", packet_data[i]);
		if ((i % 8) == 7)
		{
			printf("\n\t");
		}
	}
	printf("\n");
}

void packet::num_char_packet()
{
	unsigned char num_characters;
	unsigned char max_characters;
	disassemble(&packet_data[1], "cc", &num_characters, &max_characters);
	
	printf("You have %d of %d characters\n", num_characters, max_characters);
}

void packet::login_char_packet()
{
	char *name;
	char *pledge;
	unsigned char type;
	unsigned char gender;
	short alignment;
	short hp;
	short mp;
	char ac;
	char level;
	char str, wis, cha, con, dex, intl;
	disassemble(&packet_data[1], "sscchhhcccccccc", 
		&name, &pledge, 
		&type, &gender,
		&alignment,
		&hp, &mp, &ac, &level,
		&str, &dex, &con, &wis, &cha, &intl);
	printf("Character data:\n\tName:%s of pledge %s", name, pledge);
	printf("\n\tCharacter type:%d", type);
	printf("\n\tGender: %d", gender);
	printf("\n\tAlignment: %d", alignment);
	printf("\n\tHP: %d MP: %d AC: %d", hp, mp, ac);
	printf("\n\tLevel: %d", level);
	printf("\n\tSTR:%3d\tDEX:%3d\tCON:%3d\n\tWIS:%3d\tCHA:%3d\tINT:%3d", str, dex, con, wis, cha, intl);
	printf("\n");
	game->register_char(type*2 + gender);
}

void packet::login_check()
{
	print_packet();
}

void packet::news_packet()
{
	char *news;
	disassemble(&packet_data[1], "s", &news);
	//TODO Create class for displaying messages
	printf("The news is %s\n", news);
	reset();
	send_packet("cdd", 43, 0, 0);	//TODO: there is a minimum packet length
}

void packet::key_packet()
{
	unsigned int seed;
	disassemble(&packet_data[1], "d", &seed);
	create_key(seed);
	reset();
	//loadFile("VersionInfo", &r1_40);
	send_packet("chdcd", 0x47, 0x33, -1, 32, 101101);
	//TODO : put actual defined values here
		//acp, VersionInfo, buildNumber
		//-1, 32, 101101
}

void packet::server_version_packet()
{
	char version_check;
	unsigned int serverVersion;
	unsigned int cacheVersion;
	unsigned int authVersion;
	unsigned int npcVersion;
	unsigned int serverStartTime;
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
	//TODO : move these to an actual loading function
//	printf("Server id: %04x\n", serverId);
	if (serverId == 0x012c)
	{
		game->LoadDurationTable("spelldur300.tbl");
	}
	else
	{
		game->LoadDurationTable("spelldur.tbl");
	}
	if (countryCode == 3)
	{
		game->init_codepage(0x3b6);	//memset(lead_table, 0, 0x100); memset(lower_table, 0, 0x100); 
//		serverCP = 0x3b6;
	}
	else if (countryCode == 4)
	{
		game->init_codepage(0x3a4);
//		serverCP = 0x3a4;
	}
	else if (countryCode == 0)
	{
		game->init_codepage(0x3b5);
//		serverCP = 0;
	}
	else if (countryCode == 5)
	{
		game->init_codepage(0x3a8);
//		serverCP = 0x3a8;
	}
	else
	{
		game->init_codepage(0x4e4);
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
