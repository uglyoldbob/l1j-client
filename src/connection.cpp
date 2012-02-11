#if (_WIN32_WINNT < 0x0501)
	#warning "_WIN32_NT < 0x501"
	#undef _WIN32_WINNT
	#define _WIN32_WINNT 0x501
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "connection.h"
#include "globals.h"

int connection::connection_ok()
{
	return conn_ok;
}

int connection::change()
{
	printf("\nConnecting to game server\n");
	if (sock != -1)
	{
		closesocket(sock);
		sock = -1;
	}
	conn_ok = 0;
	try_names(the_config->get_game_port(srv_num));
	return conn_ok;
}

//TODO: implement a "waiting buffer" when sending fails ?
//TODO: limit how often this function will send data
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
		if (trans > 0)
		{
			sent += trans;
		}
	}
	return len;
}

int connection::snd_var(const void* msg, int len)
{
	if (len == 4)
	{
		unsigned int *temp = (unsigned int *)msg;
		*temp = htonl(*temp);
	}
	else if (len == 2)
	{
		unsigned short *temp = (unsigned short *)msg;
		*temp = htons(*temp);
	}
	char *buf;
	int sent = 0;
	int trans;
	while (sent < len)
	{
		buf = (char*)msg;
		buf = &buf[sent];
		trans = send(sock, buf, len-sent, 0);
		if (trans > 0)
		{
			sent += trans;
		}
	}
	return len;
}

int connection::snd_varg(const void* msg, int len)
{
	if (len == 4)
	{
		unsigned int *temp = (unsigned int *)msg;
		*temp = SWAP32(*temp);
	}
	else if (len == 2)
	{
		unsigned short *temp = (unsigned short *)msg;
		*temp = SWAP16(*temp);
	}
	char *buf;
	int sent = 0;
	int trans;

	while (sent < len)
	{
		buf = (char*)msg;
		buf = &buf[sent];
		trans = send(sock, buf, len-sent, 0);
		if (trans > 0)
		{
			sent += trans;
		}
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
		if (trans > 0)
		{
			recvd += trans;
		}
	}

	return len;
}

int connection::rcv_var(void *buf, int len)
{
	int recvd = 0;
	int trans;
	char *temp;
	while (recvd < len)
	{
		temp = (char*)buf;
		temp = &temp[recvd];
		trans = recv(sock, temp, len-recvd, 0);
		if (trans > 0)
		{
			recvd += trans;
		}
	}
	
	if (len == 4)
	{
		unsigned int *temp = (unsigned int *)buf;
		*temp = ntohl(*temp);
	}
	else if (len == 2)
	{
		unsigned short *temp = (unsigned short *)buf;
		*temp = ntohs(*temp);
	}

	return len;
}

int connection::rcv_varg(void *buf, int len)
{
	int recvd = 0;
	int trans;
	char *temp;
	while (recvd < len)
	{
		temp = (char*)buf;
		temp = &temp[recvd];
		trans = recv(sock, temp, len-recvd, 0);
		if (trans > 0)
		{
			recvd += trans;
		}
		else
		{
			return 0;
		}
	}
	
	if (len == 4)
	{
		unsigned int *temp = (unsigned int *)buf;
		*temp = SWAP32(*temp);
	}
	else if (len == 2)
	{
		unsigned short *temp = (unsigned short *)buf;
		*temp = SWAP16(*temp);
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
//			perror("ERROR: connect");
			continue;
		}

		break;
	}

	// If iMode!=0, non-blocking mode is enabled.
	u_long iMode=1;
	ioctlsocket(sock,FIONBIO,&iMode);
	
//	int x = fcntl(sock, F_GETFL, 0);
//	fcntl(sock, F_SETFL, x | O_NONBLOCK);
	
	if (p == NULL) 
	{
//		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	else
	{
		conn_ok = 1;
	}
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

connection::connection(config* lcfg, int srv_n)
{
	the_config = lcfg;
	srv_num = srv_n;
	conn_ok = 0;
	sock = -1;
	servinfo = 0;
	try_names(the_config->get_port(srv_n));
}

void connection::try_names(const char *port)
{
	for (int i = 0; i < the_config->get_num_names(srv_num); i++)
	{
		printf("\tAttempting %s [%d of %d] port %s...", 
			the_config->get_addr(srv_num, i), i+1, 
			the_config->get_num_names(srv_num), port);
		get_addr(port, the_config->get_addr(srv_num, i));
		make_connection();
		if (connection_ok() == 1)
		{
			printf(" success\n");
			break;
		}
		else
		{
			printf(" failed\n");
		}
	}

}

connection::~connection()
{
	printf("Deleting connection\n");
	if (sock != -1)
		closesocket(sock);
	if (servinfo != 0)
		freeaddrinfo(servinfo); // free the linked-list
	printf("\tDone\n");
}
