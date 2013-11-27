#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdarg.h>
#include <stdio.h>

#include "connection.h"
#include "packet_data.h"

class client;

class packet
{
	public:
		packet(client *clnt, connection *serve, sdl_user *blabla);
		~packet();
		
		void send_packet(packet_data &sendme);
		void send_packet(const char* args, ...);
		void send_packet(const char* args, va_list array);
		packet_data &get_packet(bool translate);
		void print_packet(uint8_t opcode, packet_data &printme, const char *msg);

		int process_packet();
	private:
		char decryptionKey[8];
		char encryptionKey[8];
		int mode;	//what mode is packet decoding in?
		volatile int key_initialized;
		connection *server;
		client *game;
		sdl_user *theuser;
		packet_data data;

		void encrypt(packet_data &eme);
		void decrypt(packet_data &dme);
		void change_key(char *key, const char *data);	//changes the encryption key
		void create_key(const unsigned int seed);
		
		//packet handlers
		void key_packet();
		void char_status();
		void set_map();
		void server_version_packet();
		void news_packet();
		void num_char_packet();
		void login_char_packet();
		void login_check();
		void char_create_result();
		void handle_chat(unsigned char opcode);
		void game_time();
		void update_mp();
		void update_hp();
		void ground_item();
		void place_light();
		void change_spmr();
		void weather();
		void add_inv_items();
		void server_message();
};

#endif
