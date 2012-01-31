#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdarg.h>
#include <stdio.h>

#include "connection.h"

class client;

class packet
{
	public:
		packet(client *clnt, connection *serve);
		~packet();
		
		void send_packet(const char* args, ...);
		void send_packet(const char* args, va_list array);
		void get_packet();
		void print_packet();
		void reset();
		
		int assemble(char *format, int max_length, const char *data, ...);
		int assemble(char *format, int max_length, const char *data, va_list array);
		void disassemble(unsigned char *buf, const char *format, va_list array);
		void disassemble(unsigned char *buf, const char *format, ...);
		void disass(const char *format, ...);
		
		int process_packet();
	private:
		static char decryptionKey[8];
		static char encryptionKey[8];
		static int mode;	//what mode is packet decoding in?
		static volatile int key_initialized;
		connection *server;
		client *game;
		unsigned char *packet_data;
		int packet_length;
		
		void encrypt();
		void decrypt();
		void change_key(char *key, const char *data);	//changes the encryption key
		void create_key(const unsigned int seed);
		
		//packet handlers
		void key_packet();
		void char_status();
		void set_map();
		void enter_game();
		void delete_char_packet();
		void server_version_packet();
		void news_packet();
		void num_char_packet();
		void login_char_packet();
		void login_check();
		void char_create_result();
		void handle_chat();
};

#endif
