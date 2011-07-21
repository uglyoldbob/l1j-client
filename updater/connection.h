#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <netdb.h>

#include "config.h"

class connection
{
	public:
		connection(config* lcfg);
		~connection();
		int send(const void* msg, int len);
		int recv(void *buf, int len);
	private:
		struct addrinfo *servinfo;  //used for connecting
		struct addrinfo hints;
		int sock;
		
		int get_addr(char* port, char *conto);
		int make_connection();
};

#endif