#if defined _WIN32 || defined __CYGWIN__
#ifndef _STDCALL_SUPPORTED
#define _STDCALL_SUPPORTED
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#if defined _WIN32 || defined __CYGWIN__
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/wait.h>
	#include <pthread.h>
#endif

#if defined _WIN32 || defined __CYGWIN__
int get_dword(SOCKET sockfd);
#else
int get_dword(int sockfd);
#endif

#if defined _WIN32 || defined __CYGWIN__
char* get_lots(SOCKET sockfd, int length);
#else
char* get_lots(int sockfd, int length);
#endif
 


#if defined _WIN32 || defined __CYGWIN__
int send_lots(SOCKET sockfd, char *data, unsigned int length);
#else
int send_lots(int sockfd, char *data, unsigned int length);
#endif

#if defined _WIN32 || defined __CYGWIN__
int send_dword(SOCKET sockfd, unsigned int data);
#else
int send_dword(int sockfd, unsigned int data);
#endif

#if defined _WIN32 || defined __CYGWIN__
int send_word(SOCKET sockfd, unsigned short data);
#else
int send_word(int sockfd, unsigned short data);
#endif


#if defined _WIN32 || defined __CYGWIN__
int send_byte(SOCKET sockfd, unsigned char data);
#else
int send_byte(int sockfd, unsigned char data);
#endif
