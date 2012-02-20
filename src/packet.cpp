#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "client.h"
#include "drawmode/draw_char_sel.h"
#include "drawmode/draw_game.h"
#include "drawmode/draw_new_char.h"
#include "globals.h"
#include "packet.h"
#include "unsorted.h"
#include "widgets/chat_window.h"

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

void packet::disass(const char *format, ...)
{
	va_list temp_args;
	va_start(temp_args, format);
	disassemble(packet_data, format, temp_args);
	va_end(temp_args);
}

int packet::disassemble(unsigned char *buf, const char *format, ...)
{
	int ret_val;
	va_list temp_args;
	va_start(temp_args, format);
	ret_val = disassemble(buf, format, temp_args);
	va_end(temp_args);
	return ret_val;
}

int packet::disassemble(unsigned char *buf, const char *format, va_list args)
{
	int i = 0;
	int buf_offset = 0;
	while (format[i] != 0)
	{
		switch(format[i])
		{
			case 's':
			{
				unsigned short length = 0;
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
	return buf_offset;
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
			
			//convert opcode before sending to the server
			sendbuf[2] = game->convert_client_packets[sendbuf[2]];
			
			
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

void packet::get_packet(bool translate)
{
	unsigned short length = 0;
	do
	{
		reset();
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
				if (translate)
				{
					packet_data[0] = game->convert_server_packets[packet_data[0]];
				}
			}
			else
			{
				printf("ERROR: trying to load 2 packets\n");
			}
		}
	} while ((!translate) && (length == 0));
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
	if (game->stop_thread)
		throw "Stop client thread requested";
	if (packet_data == 0)
		return 0;
	if (key_initialized == 0)
	{
		if (packet_data[0] != SERVER_KEY)
		{
			printf("This server (%d) is not compatible with this client\n", packet_data[0]);
			return -1;
		}
	}

	switch(packet_data[0])
	{	//the second list
		case SERVER_VERSIONS: server_version_packet(); break;
		case SERVER_DISCONNECT: return -1; break;
		case SERVER_CHAR_STAT: char_status(); break;
		case SERVER_TIME: game_time(); break;
		case SERVER_MPVALS: update_mp(); break;
		case SERVER_HPVALS: update_hp(); break;
		case SERVER_GROUNDITEM: ground_item(); break;
		case SERVER_LIGHT: place_light(); break;
		case SERVER_WEATHER: weather(); break;
		case SERVER_INV_ITEMS: add_inv_items(); break;
		case SERVER_MESSAGE: server_message(); break;
		case SERVER_CHANGE_SPMR: change_spmr(); break;
		case SERVER_ENTERGAME:
		case SERVER_CHAR_DELETE:
		case SERVER_LOGIN: login_check(); break;
		case SERVER_KEY: key_packet(); break;
		case SERVER_MAP: set_map(); break;
		case SERVER_NEWS: news_packet(); break;
		case SERVER_LOGIN_CHAR: login_char_packet(); break;
		case SERVER_CREATE_STAT: char_create_result();	break;
		case SERVER_NUM_CHARS: num_char_packet(); break;
		case SERVER_CHAT_NORM:
		case SERVER_CHAT_WHISPER:
		case SERVER_CHAT_GLOBAL:
		case SERVER_CHAT_SHOUT:
			handle_chat();
			break;
		default: print_packet(); break;
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
	printf("Packet data of unknown packet\n\t");
	for (int i = 0; i < packet_length; i++)
	{
		printf("%02x ", packet_data[i] & 0xff);
		if ((i % 8) == 7)
		{
			printf("\n\t");
		}
	}
	printf("\n");
}

void packet::server_message()
{
	unsigned char num_msgs;
	unsigned short offset = 1;
	unsigned short type;
	disassemble(&packet_data[offset], "hc", &type, &num_msgs);
	offset++;
	printf("There are %d messages of type %d\n", num_msgs, type);
	for (int i = 0; i < num_msgs; i++)
	{
		char *message;
		offset += disassemble(&packet_data[offset], "s", &message);
		printf("Message %d is %s\n", i, message);
		delete [] message;
	}
}

void packet::handle_chat()
{
	unsigned char type;
	int player_id;
	char *message;
	
	switch(packet_data[0])
	{
		case SERVER_CHAT_NORM:
		{
			disassemble(&packet_data[1], "cds", &type, &player_id, &message);
			chat_window *temp;
			temp = (chat_window*)(game->graphics->get_drawmode()->get_widget(1));
			temp->add_line(message);
			delete [] message;
		}
			break;
		case SERVER_CHAT_SHOUT:
		{	//605 - 612.img are the direction icons
			short x, y;
			disassemble(&packet_data[1], "cdshh", &type, &player_id, &message, &x, &y);
			chat_window *temp;
			temp = (chat_window*)(game->graphics->get_drawmode()->get_widget(1));
			temp->add_line(message);
			delete [] message;
		}
			break;
		case SERVER_CHAT_GLOBAL:
		{	
			int offset = 1;
			offset += disassemble(&packet_data[offset], "c", &type);
			switch(type)
			{
				case 9:
				{	//return from a command like .help
					disassemble(&packet_data[offset], "s", &message);
					chat_window *temp;
					temp = (chat_window*)(game->graphics->get_drawmode()->get_widget(1));
					temp->add_line(message);
					delete [] message;
				}	
					break;
				case 3:
				{	//regular global chat
					disassemble(&packet_data[offset], "s", &message);
					chat_window *temp;
					temp = (chat_window*)(game->graphics->get_drawmode()->get_widget(1));
					temp->add_line(message);
					delete [] message;
				}
					break;
				default:
					break;
			}
			
		}
			break;
		case SERVER_CHAT_WHISPER:
		{
			char *act_msg;
			char *display;
			disassemble(&packet_data[1], "ss", &message, &act_msg);
			display = new char[strlen(message) + strlen(act_msg) + 4];
			sprintf(display, "(%s) %s", message, act_msg);
			chat_window *temp;
			temp = (chat_window*)(game->graphics->get_drawmode()->get_widget(1));
			temp->add_line(display);
			delete [] message;
			delete [] act_msg;
			delete [] display;
		}
			break;
		default:
		{
			disassemble(&packet_data[1], "cds", &type, &player_id, &message);
			printf("Received chat message: %s\n", message);
			delete [] message;
		}
			break;
	}
}

void packet::add_inv_items()
{
	unsigned short offset = 1;
	
	unsigned char num_items;
	disassemble(&packet_data[offset], "c", &num_items);
	offset += 1;
	printf("Adding %d items to your inventory\n", num_items);
	for (int i = 0; i < num_items; i++)
	{
		int item_id, count;
		unsigned char use_type, blessed_status, identified, dummy;
		short icon;
		char *name;
		unsigned char status_length;
		
		offset += disassemble(&packet_data[offset], "dcchcdcsc", &item_id,
			&use_type, &dummy, &icon, &blessed_status, &count,
			&identified, &name, &status_length);
		printf("%d of Item %d usage %d (bless %d) icon %d. ", count,
			name, use_type, blessed_status, icon);
		
		if (status_length > 0)
		{
			printf("\nStatus: ");
		}
		for (int j = 0; j < status_length; j++)
		{
			unsigned char mys;
			offset += disassemble(&packet_data[offset], "c", &mys);
			printf("%02x", mys);
		}
		printf("\n");
	}
}

void packet::ground_item()
{
	unsigned short x, y, gnd_icon;
	int id, count;
	unsigned char emit_light, d1, d2, d3, d4, d5;
 	char *name;
 	disassemble(&packet_data[1], "hhdhccccdccs", &x, &y, &id, &gnd_icon,
 		&d1, &d2, &emit_light, &d3, &count, &d4, &d5, &name);
 	printf("There is %s [%d] at (%d, %d)\n", name, id, x, y); 
}

void packet::update_mp()
{
	short max_mp, cur_mp;
	disassemble(&packet_data[1], "hh", &cur_mp, &max_mp);
	game->graphics->wait_for_mode(DRAWMODE_GAME);
	draw_game *bla = (draw_game*)game->graphics->get_drawmode();
	bla->update_mpbar(cur_mp, max_mp);
}

void packet::update_hp()
{
	short max_hp, cur_hp;
	disassemble(&packet_data[1], "hh", &cur_hp, &max_hp);
	game->graphics->wait_for_mode(DRAWMODE_GAME);
	draw_game *bla = (draw_game*)game->graphics->get_drawmode();
	bla->update_hpbar(cur_hp, max_hp);
}

void packet::game_time()
{
	int time;
	disassemble(&packet_data[1], "d", &time);
//	printf("The time is %d\n", time);
}

void packet::place_light()
{
	int id;
	unsigned char type;
	disassemble(&packet_data[1], "dc", &id, &type);
	printf("Light id %d is type %d\n", id, type); 
}

void packet::change_spmr()
{
	unsigned char delta_sp, delta_mr;
	disassemble(&packet_data[1], "cc", &delta_sp, &delta_mr);
	printf("Your SP changed by %d and MR changed by %d\n", delta_sp, delta_mr);
}

void packet::weather()
{
	unsigned char theweather;
	disassemble(&packet_data[1], "c", &theweather);
	printf("The weather is %d\n", theweather);
}

void packet::set_map()
{
	short mapid;
	char underwater;
	disassemble(&packet_data[1], "hc", &mapid, &underwater);
//	printf("The map is %d and underwater:%d\n", mapid, underwater);
}

void packet::char_status()
{
	int id;
	char level;
	unsigned int exp;
	char str, intl, wis, dex, con, cha;
	short cur_hp, max_hp, cur_mp, max_mp;
	char ac;
	int time;
	char food, weight, alignment, fire_res, water_res, wind_res, earth_res;
	disassemble(&packet_data[1], "dcdcccccchhhhcdcchcccc", &id, &level, &exp, 
		&str, &intl, &wis, &dex, &con, &cha, &cur_hp, &max_hp, &cur_mp, &max_mp,
		&ac, &time, &food, &weight, &alignment, &fire_res, &water_res, &wind_res,
		&earth_res);
	printf("Character data: ID %d\n", id);
	printf("\tLevel : %d\tExp %d\n", level, exp);
	printf("\tSTR %2d CON %2d DEX %2d WIS %2d INT %2d CHA %2d\n", str, con, dex,
		wis, intl, cha);
	game->graphics->wait_for_mode(DRAWMODE_GAME);
	draw_game *bla = (draw_game*)game->graphics->get_drawmode();
	bla->update_hpbar(cur_hp, max_hp);
	bla->update_mpbar(cur_mp, max_mp);
	printf("\tAC %d\tTIME %d\tFood %d\tWeight %d\tAlignment %d\n", ac, time, 
		food, weight, alignment);
}

void packet::char_create_result()
{
	unsigned char result;
	disassemble(&packet_data[1], "c", &result);
	printf("The result from character creation is %d\n", result);
	if (result == 2)
	{
		lin_char_info *temp;
		draw_new_char* bob;
		bob = (draw_new_char*)game->graphics->get_drawmode();
		temp = bob->get_char();
		game->register_char(temp);
		game->graphics->change_drawmode(DRAWMODE_CHARSEL);
	}
}

void packet::num_char_packet()
{
	unsigned char num_characters;
	unsigned char max_characters;
	disassemble(&packet_data[1], "cc", &num_characters, &max_characters);
	
	if (max_characters == 0)
	{
		printf("WARNING : Max characters is 0 (ZERO) this is bad!\n");
		max_characters = 8;
	}
	
	game->create_chars(num_characters, max_characters, 8);
	game->register_char(0);
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
	
	lin_char_info *temp;
	temp = new lin_char_info;
	temp->name = name;
	temp->pledge = pledge;
	temp->char_type = type;
	temp->gender = gender;
	temp->alignment = alignment;
	temp->hp = hp;
	temp->mp = mp;
	temp->ac = ac;
	temp->level = level;
	temp->str = str;
	temp->dex = dex;
	temp->con = con;
	temp->wis = wis;
	temp->cha = cha;
	temp->intl = intl;
	game->register_char(temp);
}

void packet::login_check()
{
	unsigned char result;
	disassemble(&packet_data[1], "c", &result);
	
	switch (result)
	{
		case 3:
			//send 9?
			send_packet("cdd", CLIENT_ALIVE, 0, 0);
			send_packet("ccdd", CLIENT_INITGAME, 0, 0, 0);
			game->graphics->change_drawmode(DRAWMODE_GAME);
			break;
		case 5:
			{
			draw_char_sel* bob;
			bob = (draw_char_sel*)game->graphics->get_drawmode();
			bob->do_delete();
			}
			break;
		case 0x51:
			printf("Char will be deleted in the future\n");
			break;
		default:
			printf("Unhandled Login check packet (%d)\n", result);
			break;
	}
	
	print_packet();
}

void packet::news_packet()
{
	char *news;
	disassemble(&packet_data[1], "s", &news);
	//TODO Create class for displaying messages
	printf("STUB The news is %s\n", news);
	reset();
	send_packet("cdd", CLIENT_CLICK, 0, 0);	//TODO: there is a minimum packet length
	game->graphics->change_drawmode(DRAWMODE_CHARSEL);
}

void packet::key_packet()
{
	unsigned int seed;
	disassemble(&packet_data[1], "d", &seed);
	create_key(seed);
	reset();
	//loadFile("VersionInfo", &r1_40);
	send_packet("chdcd", CLIENT_VERSION, 0x33, -1, 32, 101101);
	//TODO : put actual defined values here
		//acp, VersionInfo, buildNumber
		//-1, 32, 101101
}

void packet::server_version_packet()
{
	unsigned char version_check;
	unsigned int serverVersion;
	unsigned int cacheVersion;
	unsigned int authVersion;
	unsigned int npcVersion;
	unsigned int serverStartTime;
	unsigned char canMakeNewAccount;
	unsigned char englishOnly;
	unsigned char countryCode;
	unsigned char serverCode;
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
//	printf("STUB AdjustExp()\n");
//	AdjustExp();
//	printf("STUB Modify serverCP and acp\n");
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
//	printf("STUB LoadEmblemFile()\n");
//	LoadEmblemFile(serverId);
	return;
}
