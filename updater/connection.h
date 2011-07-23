#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <netdb.h>

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
		int sock;
		int conn_ok;
		
		int get_addr(const char* port, const char *conto);
		int make_connection();
};

#endif