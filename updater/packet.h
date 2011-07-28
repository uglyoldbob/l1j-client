#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdarg.h>
#include <stdio.h>

#include "connection.h"

class packet
{
	public:
		packet(connection *serve);
		~packet();
		
	private:
		static char decryptionKey[8];
		static char encryptionKey[8];
		static int mode;	//what mode is packet decoding in?
		static int key_initialized;
		connection *server;
		unsigned char *packet_data;
		int packet_length;
		
		void sendPacket(const char* args, ...);
		void getPacket(const char* args, ...);
		
		int assemble(char *format, int max_length, const char *data, ...);
		int assemble(char *format, int max_length, const char *data, va_list array);
		void disassemble(unsigned char *buf, const char *format, ...);
		
		void processPacket();	//process packet data and return the proper sub-class
		
		void encrypt();
		void decrypt();
		void change_key(char *key, const char *data);	//changes the encryption key
		void create_key(const unsigned long seed);
		void reset();
		
		//packet handlers
		void keyPacket();
		void serverVersionPacket();
};

#endif
