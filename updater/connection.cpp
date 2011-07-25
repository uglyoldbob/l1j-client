#if (_WIN32_WINNT < 0x0501)
	#warning "_WIN32_NT < 0x501"
	#undef _WIN32_WINNT
	#define _WIN32_WINNT 0x501
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "connection.h"

int connection::connection_ok()
{
	return conn_ok;
}

int connection::change()
{
//	if (sock != -1)
//		closesocket(sock);
	return -1;
}

int connection::snd(const void* msg, int len)
{
	char *buf;
	int sent = 0;
	int trans;
	while (sent < len)
	{
		buf = (char*)msg;
		buf = &buf[sent];
		trans = send(sock, buf, len-sent, 0);
		if (trans < 0)
		{
			printf("Error sending %d \n", errno);
			throw 2;
			return trans;
		}
		sent += trans;
	}
	return len;
}

int connection::rcv(void *buf, int len)
{
	int recvd = 0;
	int trans;
	char *temp;
	while (recvd < len)
	{
		temp = (char*)buf;
		temp = &temp[recvd];
		trans = recv(sock, temp, len-recvd, 0);
		if (trans < 0)
		{
			printf("Error receiving %d\n", errno);
			throw 1;
			return recvd;
		}
		recvd += trans;
	}
	return len;
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
			closesocket(sock);
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
	conn_ok = 1;
	return 0;
}

int connection::get_addr(const char* port, const char* conto)
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
	conn_ok = 0;
	sock = -1;
	//TODO: implement error checking
	get_addr(lcfg->get_port(), lcfg->get_addr(0));
	make_connection();
}

connection::~connection()
{
	printf("Deleting connection\n");
//	if (sock != -1)
//		closesocket(sock);
//	if (servinfo != 0)
//		freeaddrinfo(servinfo); // free the linked-list
	printf("\tDone\n");
}
