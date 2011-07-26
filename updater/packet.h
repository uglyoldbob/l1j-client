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
		connection *server;
		char *packet_data;
		int packet_length;
		
		void sendPacket(const char* args, ...);
		int assemble(char *format, int max_length, const char *data, ...);
		int assemble(char *format, int max_length, const char *data, va_list array);
		
		void encrypt();
		void decrypt();
		void change_key(char *data);	//changes the encryption key
};

#endif
