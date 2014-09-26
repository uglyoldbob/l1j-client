#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "drawmode/draw_char_sel.h"
#include "drawmode/draw_game.h"
#include "drawmode/draw_new_char.h"
#include "globals.h"
#include "packet.h"
#include "widgets/chat_window.h"

static const int MAX_LENGTH = 0x13fe;

void packet::send_packet(packet_data &sendme)
{
	while (key_initialized == 0)
	{	/** \todo Indicate to user that we are waiting on the server */
		SDL_Delay(100);
	}

	//convert opcode before sending to the server
	sendme[0] = theuser->convert_client_packets[sendme[0]];

	//key for changing encryption is retrieved and put back
	unsigned char key_change[4];
	key_change[0] = sendme[0];
	key_change[1] = sendme[1];
	key_change[2] = sendme[2];
	key_change[3] = sendme[3];

//	print_packet(0, sendme, "Sending packet");
	//packet is prepended with packet length
	sendme.insert((uint16_t)(sendme.size()+2));
	this->encrypt(sendme);
	this->change_key(encryptionKey, (char*)key_change);
	server->snd(sendme);
}

packet_data &packet::get_packet(bool translate)
{
	unsigned short length = 0;
	do
	{
		data.clear();
		server->rcv_varg(&length, 2);
		if (length != 0)
		{
 			if (data.size() == 0)
			{
				unsigned int packet_length = length - 2;
				unsigned char *packet_datas = new unsigned char[packet_length];
				server->rcv(packet_datas, packet_length);
				data = std::vector<unsigned char>(packet_datas, packet_datas+packet_length);
				delete [] packet_datas;
				packet_datas = 0;
				if (key_initialized == 1)
				{
					this->decrypt(data);
					char key_change[4];
					key_change[0] = data[0];
					key_change[1] = data[1];
					key_change[2] = data[2];
					key_change[3] = data[3];
					this->change_key(decryptionKey, key_change);
				}	
				if (translate)
				{
					uint8_t temp = data[0];
					data[0] = theuser->convert_server_packets[temp];
					if (data[0] == 255)
						printf("A PACKET (%d) WAS untranslated\n", temp);
				}
			}
			else
			{
				printf("ERROR: trying to load 2 packets\n");
			}
		}
	} while ((!translate) && (length == 0));
	return data;
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

void packet::encrypt(packet_data &eme)
{
	if (eme.size() != 0)
	{
		eme[2] ^= encryptionKey[0];
		
		for (int i = 3; i < eme.size(); i++)
		{
			eme[i] ^= encryptionKey[(i-2) & 7] ^ eme[i-1];
		}

		eme[5] ^= encryptionKey[2];
		eme[4] ^= encryptionKey[3] ^ eme[5];
		eme[3] ^= encryptionKey[4] ^ eme[4];
		eme[2] ^= encryptionKey[5] ^ eme[3];
	}
}

void packet::decrypt(packet_data &dme)
{
	if (dme.size() != 0)
	{
		char b3 = dme[3];
		dme[3] ^= decryptionKey[2];

		char b2 = dme[2];
		dme[2] ^= (b3 ^ decryptionKey[3]);

		char b1 = dme[1];
		dme[1] ^= (b2 ^ decryptionKey[4]);

		char k = dme[0] ^ b1 ^ decryptionKey[5];
		dme[0] = k ^ decryptionKey[0];

		for (int i = 1; i < dme.size(); i++) 
		{
			char t = dme[i];
			dme[i] ^= (decryptionKey[i & 7] ^ k);
			k = t;
		}
	}
}

packet::packet(connection *serve, sdl_user *blabla)
{
	//merely copy the existing connection so we can use it
	server = serve;
	mode = 0;
	key_initialized = 0;
	theuser = blabla;
}

int packet::process_packet()
{
	if (data.size() == 0)
		return 0;
	if (key_initialized == 0)
	{
		if (data[0] != SERVER_KEY)
		{
			printf("This server (%d) is not compatible with this client\n", data[0]);
			return -1;
		}
	}

	unsigned char temp;
//	print_packet(0, data, "Received packet");
	data >> temp;
	switch(temp)
	{	//the second list
		case SERVER_VERSIONS: server_version_packet(); break;
		case SERVER_DISCONNECT: return -1; break;
		case SERVER_CHAR_STAT: char_status(); break;
		case SERVER_CHAR_ALIGNMENT: char_alignment(); break;
		case SERVER_TIME: game_time(); break;
		case SERVER_MPVALS: update_mp(); break;
		case SERVER_HPVALS: update_hp(); break;
		case SERVER_GROUNDITEM: ground_item(); break;
		case SERVER_LIGHT: place_light(); break;
		case SERVER_WEATHER: weather(); break;
		case SERVER_INV_ITEMS: add_inv_items(); break;
		case SERVER_ITEM_BLESS_STATUS: item_bless_status(); break;
		case SERVER_MESSAGE: server_message(); break;
		case SERVER_CHANGE_SPMR: change_spmr(); break;
		case SERVER_MOVE_OBJECT: move_object(); break;
		case SERVER_CHANGE_HEADING: change_heading(); break;
		case SERVER_REMOVE_OBJECT: remove_object(); break;
		case SERVER_ENTERGAME:
		case SERVER_CHAR_DELETE:
		case SERVER_LOGIN: login_check(); break;
		case SERVER_KEY: key_packet(); break;
		case SERVER_MAP: set_map(); break;
		case SERVER_NEWS: news_packet(); break;
		case SERVER_LOGIN_CHAR: login_char_packet(); break;
		case SERVER_CREATE_STAT: char_create_result();	break;
		case SERVER_DEX_UPDATE: dex_update(); break;
		case SERVER_STR_UPDATE: str_update(); break;
		case SERVER_NUM_CHARS: num_char_packet(); break;
		case SERVER_TITLE: char_title(); break;
		case SERVER_AC_ELEMENTAL_UPDATE: ac_and_elemental_update(); break;
		case SERVER_CHAT_NORM:
		case SERVER_CHAT_WHISPER:
		case SERVER_CHAT_GLOBAL:
		case SERVER_CHAT_SHOUT:
			handle_chat(temp);
			break;
		default: print_packet(temp, data, "unknown packet"); break;
	}
	data.clear();
	return 0;
}

packet::~packet()
{
}

void packet::print_packet(uint8_t opcode, packet_data &printme, const char *msg)
{
	printf("********\nPacket data of %s (0x%02x %d)\n\t", msg, opcode, opcode);
	for (int i = 0; i < printme.size(); i++)
	{
		printf("%02x ", printme[i] & 0xff);
		if ((i % 8) == 7)
		{
			printf("\n\t");
		}
	}
	printf("\n********\n");
}

void packet::ac_and_elemental_update()
{
	int8_t ac, fire, water, earth, wind;
	data >> ac >> fire >> water >> wind >> earth;
	printf("Update AC:%d, fire:%d, water:%d, wind:%d, earth:%d\n", ac, fire, water, wind, earth);
}

void packet::dex_update()
{
	uint16_t time = 0;
	uint8_t dex = 0, type = 0;
	data >> time >> dex >> type;
	printf("Dexterity was updated to %d at %d, code 0x%02x\n", dex, time, type);
}

void packet::str_update()
{
	uint16_t time;
	uint8_t str, capacity, type;
	data >> time >> str >> capacity >> type;
	printf("Strength updated to %d (%d) at %d, code 0x%02x\n", str, capacity, time, type);
}

void packet::char_title()
{
	uint32_t id;
	char *name;
	data >> id >> name;
	printf("%d has a title of \"%s\"\n", id, name);
	delete [] name;
	name = 0;
}

void packet::char_alignment()
{
	unsigned char align;
	uint32_t id;
	
	data >> id >> align;
	printf("[0x%x] Char alignment is:%d\n", id, align);
}

void packet::server_message()
{
	unsigned char num_msgs;
	unsigned short offset = 1;
	unsigned short type;
	data >> type >> num_msgs;
	printf("There are %d messages of type %d\n", num_msgs, type);
	for (int i = 0; i < num_msgs; i++)
	{
		char *message;
		data >> message;
		printf("Message %d is %s\n", i, message);
		delete [] message;
		message = 0;
	}
}

void packet::handle_chat(unsigned char opcode)
{
	unsigned char type;
	uint32_t player_id;
	char *message;
	
	switch(opcode)
	{
		case SERVER_CHAT_NORM:
		{
			data >> type >> player_id >> message;
			if (theuser->is_in_mode(DRAWMODE_GAME, true))
			{
				chat_window *temp;
				temp = (chat_window*)(theuser->get_drawmode(false)->get_widget(1));
				temp->add_line(message);
				temp = 0;
				theuser->done_with_drawmode();
			}
			delete [] message;
			message = 0;
		}
			break;
		case SERVER_CHAT_SHOUT:
		{	//605 - 612.img are the direction icons
			short x, y;
			data >> type >> player_id >> message >> x >> y;
			if (theuser->is_in_mode(DRAWMODE_GAME, true))
			{
				chat_window *temp;
				temp = (chat_window*)(theuser->get_drawmode(false)->get_widget(1));
				temp->add_line(message);
				temp = 0;
				theuser->done_with_drawmode();
			}
			delete [] message;
			message = 0;
		}
			break;
		case SERVER_CHAT_GLOBAL:
		{	
			data >> type;
			switch(type)
			{
				case 9:
				{	//return from a command like .help
					data >> message;
					if (theuser->is_in_mode(DRAWMODE_GAME, true))
					{
						chat_window *temp;
						temp = (chat_window*)(theuser->get_drawmode(false)->get_widget(1));
						temp->add_line(message);
						temp = 0;
						theuser->done_with_drawmode();
					}
					delete [] message;
					message = 0;
				}	
					break;
				case 3:
				{	//regular global chat
					data >> message;
					if (theuser->is_in_mode(DRAWMODE_GAME, true))
					{
						chat_window *temp;
						temp = (chat_window*)(theuser->get_drawmode(false)->get_widget(1));
						temp->add_line(message);
						temp = 0;
						theuser->done_with_drawmode();
					}
					delete [] message;
					message = 0;
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
			data >> message >> act_msg;
			display = new char[strlen(message) + strlen(act_msg) + 4];
			sprintf(display, "(%s) %s", message, act_msg);
			if (theuser->is_in_mode(DRAWMODE_GAME, true))
			{
				chat_window *temp;
				temp = (chat_window*)(theuser->get_drawmode(false)->get_widget(1));
				temp->add_line(message);
				temp = 0;
				theuser->done_with_drawmode();
			}
			delete [] message;
			message = 0;
			delete [] act_msg;
			act_msg = 0;
			delete [] display;
			display = 0;
		}
			break;
		default:
		{
			data >> type >> player_id >> message;
			printf("Received chat message: %s\n", message);
			delete [] message;
			message = 0;
		}
			break;
	}
}

void packet::item_bless_status()
{
	unsigned char bless;
	uint32_t id;
	data >> id >> bless;
	printf("Item [0x%x] is blessed %d\n", id, bless);
}

void packet::add_inv_items()
{
	unsigned char num_items;
	data >> num_items;
	printf("Adding %d items to your inventory\n", num_items);
	for (int i = 0; i < num_items; i++)
	{
		int item_id, count;
		unsigned char use_type, blessed_status, identified, dummy;
		short icon;
		char *name;
		unsigned char status_length;
		data >> item_id >> use_type >> dummy >> icon >> blessed_status
			 >> count >> identified >> name >> status_length;
		printf("%d of Item %s usage %d (bless %d) icon %d. ", count,
			name, use_type, blessed_status, icon);
		
		if (status_length > 0)
		{
			printf("\n\tStatus: ");
		}
		for (int j = 0; j < status_length; j++)
		{
			unsigned char mys;
			data >> mys;
			printf("%02x", mys);
		}
		delete [] name;
		name = 0;
		printf("\n");
	}
}

void packet::remove_object()
{
	uint32_t id;
	data >> id;
	if (theuser->is_in_mode(DRAWMODE_GAME, true))
	{
		draw_game *temp;
		temp = (draw_game*)(theuser->get_drawmode(false));
		temp->remove_character(id);
		temp = 0;
		theuser->done_with_drawmode();
	}
}

void packet::change_heading()
{
	uint32_t id;
	uint8_t heading;
	data >> id >> heading;
	if (theuser->is_in_mode(DRAWMODE_GAME, true))
	{
		draw_game *temp;
		struct ground_item placeme;
		temp = (draw_game*)(theuser->get_drawmode(false));
		temp->change_heading(id, heading);
		temp = 0;
		theuser->done_with_drawmode();
	}
}

void packet::move_object()
{
	uint32_t id;
	uint16_t x, y;
	uint8_t heading;
	data >> id >> x >> y >> heading;
	if (theuser->is_in_mode(DRAWMODE_GAME, true))
	{
		draw_game *temp;
		struct ground_item placeme;
		temp = (draw_game*)(theuser->get_drawmode(false));
		placeme.name = 0;
		placeme.x = x;
		placeme.y = y;
		placeme.emit_light = 0;
		placeme.gnd_icon = 0;
		placeme.heading = heading;
		placeme.count = 0;
		placeme.id = id;
		temp->place_character(&placeme);
		temp = 0;
		theuser->done_with_drawmode();
	}
}\

void packet::ground_item()
{
	uint16_t x, y, gnd_icon;
	uint32_t id, count;
	uint8_t emit_light, d1, d2, d3, d4, d5;
 	char *name;
	data >> x >> y >> id >> gnd_icon >> d1 >> d2 >> emit_light >> d3
		 >> count >> d4 >> d5 >> name;
 	printf("There is %s [0x%x] at (%d, %d) icon %d : %d, %d, %d, %d, %d\n", name, id, x, y, gnd_icon,
		d1, d2, d3, d4, d5);
	if (theuser->is_in_mode(DRAWMODE_GAME, true))
	{
		draw_game *temp;
		struct ground_item placeme;
		temp = (draw_game*)(theuser->get_drawmode(false));
		placeme.name = name;
		placeme.x = x;
		placeme.y = y;
		placeme.emit_light = emit_light;
		placeme.heading = d2;
		placeme.gnd_icon = gnd_icon;
		placeme.count = count;
		placeme.id = id;
		temp->place_character(&placeme);
		temp = 0;
		theuser->done_with_drawmode();
	}
	delete [] name;
	name = 0;
}

void packet::update_mp()
{
	short max_mp, cur_mp;
	data >> cur_mp >> max_mp;
	if (theuser->is_in_mode(DRAWMODE_GAME, true))
	{
		draw_game *temp;
		temp = (draw_game*)(theuser->get_drawmode(false));
		temp->update_mpbar(cur_mp, max_mp);
		temp = 0;
		theuser->done_with_drawmode();
	}
}

void packet::update_hp()
{
	short max_hp, cur_hp;
	data >> cur_hp, max_hp;
	if (theuser->is_in_mode(DRAWMODE_GAME, true))
	{
		draw_game *temp;
		temp = (draw_game*)(theuser->get_drawmode(false));
		temp->update_hpbar(cur_hp, max_hp);
		temp = 0;
		theuser->done_with_drawmode();
	}
}

void packet::game_time()
{
	int time;
	data >> time;
//	printf("The time is %d\n", time);
}

void packet::place_light()
{
	int id;
	unsigned char type;
	data >> id >> type;
	printf("Light id %d is type %d\n", id, type); 
}

void packet::change_spmr()
{
	unsigned char delta_sp, delta_mr;
	data >> delta_sp >> delta_mr;
	printf("Your SP changed by %d and MR changed by %d\n", delta_sp, delta_mr);
}

void packet::weather()
{
	unsigned char theweather;
	data >> theweather;
	printf("The weather is %d\n", theweather);
}

void packet::set_map()
{
	short mapid;
	uint8_t underwater;
	data >> mapid >> underwater;
	printf("The map is %d and underwater:%d\n", mapid, underwater);
	if (theuser->is_in_mode(DRAWMODE_GAME, true))
	{
		draw_game *temp;
		temp = (draw_game*)(theuser->get_drawmode(false));
		temp->set_underwater(underwater);
		temp->change_map(mapid);
		temp = 0;
		theuser->done_with_drawmode();
	}
}

void packet::char_status()
{
	uint32_t id;
	uint8_t level;
	unsigned int exp;
	uint8_t str, intl, wis, dex, con, cha;
	short cur_hp, max_hp, cur_mp, max_mp;
	int8_t ac;
	int time;
	int8_t food, weight, alignment, fire_res, water_res, wind_res, earth_res;
	data >> id >> level >> exp 
		 >> str >> intl >> wis >> dex >> con >> cha
		 >> cur_hp >> max_hp >> cur_mp >> max_mp
		 >> ac >> time >> food >> weight >> alignment
		 >> fire_res >> water_res >> wind_res >> earth_res;
	printf("Character data: ID %x\n", id);
	printf("\tLevel : %d\tExp %d\n", level, exp);
	printf("\tSTR %2d CON %2d DEX %2d WIS %2d INT %2d CHA %2d\n", str, con, dex,
		wis, intl, cha);

	if (theuser->is_in_mode(DRAWMODE_GAME, true))
	{
		draw_game *temp;
		temp = (draw_game*)(theuser->get_drawmode(false));
		temp->update_hpbar(cur_hp, max_hp);
		temp->update_mpbar(cur_mp, max_mp);
		temp->set_player_id(id);
		temp = 0;
		theuser->done_with_drawmode();
	}
	printf("\tAC %d\tTIME %d\tFood %d\tWeight %d\tAlignment %d\n", ac, time, 
		food, weight, alignment);
}

void packet::char_create_result()
{
	unsigned char result;
	data >> result;
	printf("The result from character creation is %d\n", result);
	if (result == 2)
	{
		if (theuser->is_in_mode(DRAWMODE_NEWCHAR, true))
		{
			lin_char_info *tempchar;
			draw_new_char* temp;
			temp = (draw_new_char*)(theuser->get_drawmode(false));
			tempchar = temp->get_char();
			theuser->register_char(tempchar);
			tempchar = 0;
			temp = 0;
			theuser->done_with_drawmode();
		}
		theuser->change_drawmode(DRAWMODE_CHARSEL);
	}
}

void packet::num_char_packet()
{
	unsigned char num_characters;
	unsigned char max_characters;
	data >> num_characters >> max_characters;
	
	if (max_characters == 0)
	{
		printf("WARNING : Max characters is 0 (ZERO) this is bad!\n");
		max_characters = 8;
	}
	
	theuser->create_chars(num_characters, max_characters, 8);
	theuser->register_char(0);
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
	int8_t ac;
	int8_t level;
	int8_t str, wis, cha, con, dex, intl;
	data >> name >> pledge >> type >> gender >> alignment
		 >> hp >> mp >> ac >> level
		 >> str >> dex >> con >> wis >> cha >> intl;
	
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
	theuser->register_char(temp);
}

void packet::login_check()
{
	unsigned char result;
	data >> result;
	
	switch (result)
	{
		case 3:
			//send 9?
			{
			lin_char_info *picked_char;
			if (theuser->is_in_mode(DRAWMODE_CHARSEL, true))
			{
				draw_char_sel* temp;
				temp = (draw_char_sel*)theuser->get_drawmode(false);
				picked_char = temp->get_selected_char();
				theuser->done_with_drawmode();
			}
			packet_data sendme;
			sendme << (uint8_t)CLIENT_ALIVE << (uint32_t)0 << (uint32_t)0;
			send_packet(sendme);
			sendme.clear();
			sendme << (uint8_t)CLIENT_INITGAME << (uint8_t)0 << (uint32_t)0 << (uint32_t)0;
			send_packet(sendme);
			theuser->change_drawmode(DRAWMODE_GAME);
			theuser->wait_for_mode(DRAWMODE_GAME, true);
			draw_game *tempest = (draw_game*)theuser->get_drawmode(false);
			tempest->set_selected_char(picked_char);
			theuser->done_with_drawmode();
			}
			break;
		case 5:
			{
			if (theuser->is_in_mode(DRAWMODE_CHARSEL, true))
			{
				draw_char_sel* temp;
				temp = (draw_char_sel*)theuser->get_drawmode(false);
				temp->do_delete();
				theuser->done_with_drawmode();
			}
			}
			break;
		case 0x51:
			printf("Char will be deleted in the future\n");
			break;
		default:
			print_packet(result, data, "unknown login check packet");
			break;
	}
}

void packet::news_packet()
{
	char *news;
	data >> news;
	//TODO Create class for displaying messages
	printf("STUB The news is %s\n", news);
	delete [] news;
	news = 0;
	data.clear();
	packet_data sendme;
	sendme << (uint8_t) CLIENT_CLICK << (uint32_t)0 << (uint32_t)0;
	send_packet(sendme);	/** \TODO: there is a minimum packet length */
	theuser->change_drawmode(DRAWMODE_CHARSEL);
}

void packet::key_packet()
{
	unsigned int seed;
	data >> seed;
	printf("The key is 0x%x\n", seed);
	create_key(seed);
	data.clear();
	//loadFile("VersionInfo", &r1_40);
	packet_data sendme;
	sendme << (uint8_t)CLIENT_VERSION
		<< (uint16_t)0x33
		<< (uint32_t)-1
		<< (uint8_t)32
		<< (uint32_t)101101;
	send_packet(sendme);
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
	data >> version_check;
	if (version_check == 1)
	{
		printf("ERROR: Load version data from somewhere else\n");
	}
	else
	{
		data >> serverCode >> serverVersion >> cacheVersion >> authVersion
			 >> npcVersion >> serverStartTime >> canMakeNewAccount >> englishOnly;
	}
	data >> countryCode;
	
	unsigned short serverId = (countryCode<<8) + serverCode;
	//TODO : move these to an actual loading function
//	printf("Server id: %04x\n", serverId);
	if (serverId == 0x012c)
	{
		theuser->LoadDurationTable("spelldur300.tbl");
	}
	else
	{
		theuser->LoadDurationTable("spelldur.tbl");
	}
	if (countryCode == 3)
	{
		theuser->init_codepage(0x3b6);	//memset(lead_table, 0, 0x100); memset(lower_table, 0, 0x100); 
//		serverCP = 0x3b6;
	}
	else if (countryCode == 4)
	{
		theuser->init_codepage(0x3a4);
//		serverCP = 0x3a4;
	}
	else if (countryCode == 0)
	{
		theuser->init_codepage(0x3b5);
//		serverCP = 0;
	}
	else if (countryCode == 5)
	{
		theuser->init_codepage(0x3a8);
//		serverCP = 0x3a8;
	}
	else
	{
		theuser->init_codepage(0x4e4);
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

