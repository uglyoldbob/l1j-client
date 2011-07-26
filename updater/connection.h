#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "config.h"

class connection
{
	public:
		connection(config* lcfg);
		~connection();
		int connection_ok();
		int snd(const void* msg, int len);
		int rcv(void *buf, int len);
		int change();	//changes from update connection to game connection
		
		int snd_var(const void* msg, int len);
		int rcv_var(void *buf, int len);
	private:
		struct addrinfo *servinfo;  //used for connecting
		struct addrinfo hints;
		config *the_config;
		int sock;
		int conn_ok;
		
		int get_addr(const char* port, const char *conto);
		int make_connection();
		void try_names(const char *port);
};

#endif
