#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#ifdef HAVE_CONFIG_H
#include <ac_config.h>
#endif

#ifdef WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

#include "config.h"
#include "packet_data.h"

class connection
{
	public:
		connection(config* lcfg, int srv_n);
		~connection();
		int connection_ok();
		int snd(const void* msg, int len);
		int snd(packet_data &sendme);
		int rcv(void *buf, int len);
		int change();	//changes from update connection to game connection
		
		//sends and receives network order information
		int snd_var(const void* msg, int len);
		int rcv_var(void *buf, int len);
		
		//sends and receives backwards network order information (for the game server)
		int snd_varg(const void* msg, int len);
		int rcv_varg(void *buf, int len);
	private:
		struct addrinfo *servinfo;  //used for connecting
		struct addrinfo hints;
		config *the_config;
		#ifdef WINDOWS
			SOCKET sock;
		#else
			int sock;
		#endif
		int conn_ok;
		int srv_num;
		
		int get_addr(const char* port, const char *conto);
		int make_connection();
		void try_names(const char *port);
};

#endif

