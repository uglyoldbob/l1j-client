#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <winsock2.h>
#include <ws2tcpip.h>

#include "config.h"

class connection
{
	public:
		connection(config* lcfg);
		~connection();
		int connection_ok();
		int snd(const void* msg, int len);
		int rcv(void *buf, int len);
	private:
		struct addrinfo *servinfo;  //used for connecting
		struct addrinfo hints;
		SOCKET sock;
		int conn_ok;
		
		int get_addr(const char* port, const char *conto);
		int make_connection();
};

#endif