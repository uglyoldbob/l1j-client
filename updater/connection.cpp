#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "connection.h"


int connection::send(const void* msg, int len)
{
	return send(sock, msg, len, 0);
}

int connection::recv(void *buf, int len)
{
	return recv(sock, buf, len, 0);
}

int connection::make_connection()
{
	struct addrinfo *p;
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		if ((sock = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) 
		{
			perror("ERROR: cannot create socket\n");
			continue;
		}

		if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sock);
			perror("ERROR: connect");
			continue;
		}

		break;
	}

	if (p == NULL) 
	{
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	
	return 0;
}

int connection::get_addr(char* port, char* conto)
{
	int status;
			
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	
	if ((status = getaddrinfo(conto, port, &hints, &servinfo)) != 0)
	{
	    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
	    return -1;
	}
	return 0;
}

connection::connection(config* lcfg)
{
	sock = -1;
	//TODO: implement error checking
	get_addr(lcfg->get_port(), lcfg->get_addr(0));
	make_connection();
}

connection::~connection()
{
	if (sock != -1)
		close(sock);
	if (servinfo != 0)
		freeaddrinfo(servinfo); // free the linked-list
}