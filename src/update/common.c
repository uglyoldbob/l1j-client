#if defined _WIN32 || defined __CYGWIN__
#ifndef _STDCALL_SUPPORTED
#define _STDCALL_SUPPORTED
#endif
#endif


#include <stdio.h>

#include "common.h"

#if defined _WIN32 || defined __CYGWIN__
int get_dword(SOCKET sockfd)
#else
int get_dword(int sockfd)
#endif
{
	int buffer;
	if ((recv(sockfd, (void*)&buffer, 4, 0)) == -1) 
	{
		printf("recv error\n");
		return -1;
	}
//	printf("%X\n", buffer[0]);
	return (int)ntohl(buffer);
}

#if defined _WIN32 || defined __CYGWIN__
char* get_lots(SOCKET sockfd, int length)
#else
char* get_lots(int sockfd, int length)
#endif
{
	char *buffer = (char *) malloc (length + 1);
	if ((recv(sockfd, buffer, length, 0)) == -1)
	{
		printf("get_lots error\n");
		return 0;
	}
//	printf("%X\n", buffer[0]);
	return buffer;
}

#if defined _WIN32 || defined __CYGWIN__
int send_lots(SOCKET sockfd, char *data, unsigned int length)
#else
int send_lots(int sockfd, char *data, unsigned int length)
#endif
{
	int c;
//	printf("Send: ");
//	for (c = 0; c < length; c++)
//	{
//		printf ("%x,", data[c] & 0xFF);
//	} 
	if (send(sockfd, data, length, 0) == -1)
	{
		printf("Error sending to client\n");
		return -1;
	}
//	printf("\n");
	return 0;
}

#if defined _WIN32 || defined __CYGWIN__
int send_dword(SOCKET sockfd, unsigned int data)
#else
int send_dword(int sockfd, unsigned int data)
#endif
{
	unsigned int temp = htonl(data);
	
//	printf ("Send: %x\n", data);
	if (send(sockfd, (unsigned char*)&temp, 4, 0) == -1)
	{
		printf("Error sending to client\n");
		return -1;
	}
	return 0;
}

#if defined _WIN32 || defined __CYGWIN__
int send_word(SOCKET sockfd, unsigned short data)
#else
int send_word(int sockfd, unsigned short data)
#endif
{
	unsigned short temp = htons(data);
//	printf ("Send: %x\n", data);
	if (send(sockfd, (unsigned char*)&temp, 2, 0) == -1)
	{
		printf("Error sending to client\n");
		return -1;
	}
	return 0;
}

#if defined _WIN32 || defined __CYGWIN__
int send_byte(SOCKET sockfd, unsigned char data)
#else
int send_byte(int sockfd, unsigned char data)
#endif
{
//	printf ("Send: %x\n", data);
	if (send(sockfd, &data, 1, 0) == -1)
	{
		printf("Error sending to client\n");
		return -1;
	}
	return 0;
}

