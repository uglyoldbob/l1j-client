#if defined _WIN32 || defined __CYGWIN__
#ifndef _STDCALL_SUPPORTED
#define _STDCALL_SUPPORTED
#endif
#endif

#include "common.h"
#include "time.h"

char *Port = "2003";  // the port users will be connecting to
	//2003

#define BACKLOG 10	 // how many pending connections queue will hold

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

struct update_block
{	//the last block in the array will match all checksums not previously caught
	unsigned long checksum;		//the checksum for this element
	char* *filenames;			//an array of filenames
	unsigned long *new_cs;		//the new "version" after that particular file is downloaded
	int *name_length;			//an array storing the length of each filename
	unsigned long *file_size;	//stores the filesize for each file
	int number_files;			//the number of files
};

//these two variables are used to store what files the server distributes
//they are used among several threads
struct update_block *updates;
int number_updates;

#if defined _WIN32 || defined __CYGWIN__
void give_file(char *filename, SOCKET send_to, unsigned int new_cs)
#else
void give_file(char *filename, int send_to, unsigned int new_cs)
#endif
{
	char buffer[1024];
	char *sys_name;
	int sys_name_length = strlen(filename) + 6;
		//./'filename'.gz (five extra letters, plus the null at the end
	sys_name = malloc (sys_name_length);
	strcpy(sys_name, "./");
	strcat(sys_name, filename);
	strcat(sys_name, ".gz");
	FILE *archive = fopen(sys_name, "rb");

	if (archive == NULL)
	{
		printf("Null file to send: %s\n", sys_name);
		return;
	}
	
	//length of the filename
	send_byte(send_to, strlen(filename));
	
	//send filename
	send_lots(send_to, filename, strlen(filename));
	
	//send filesize
	int size;
	fseek(archive, 0, SEEK_END);
	size = ftell(archive);
	fseek(archive, 0, SEEK_SET);
	send_dword(send_to, size);


	printf("Sending %s, name length %d, filesize %d\n",
		filename, strlen(filename), size);

	printf ("%s: %s size %i\n", sys_name, filename, size);
	
	int size_left = size;
	while (!feof(archive))
	{
		fread(buffer, sizeof(char), sizeof(buffer)/sizeof(char), archive);
		if (size_left > 1024)
		{
			send_lots(send_to, buffer, 1024);
			size_left -= 1024;
		}
		else
		{
			send_lots(send_to, buffer, size_left);
		}
		
	}
	
	send_dword(send_to, new_cs);	//"1234"
		//this updates the "version" of the client
	
	fclose(archive);
}

//format of the configuration file
//# as the first character of a line is a comment
//Port=2003		not implemented
//BindAddr=*	not implemented
//NumVersions=3			number of checksum entries
//Checksum=b7fdb9c0		(checksum to match for as sent from the client)
//	FileCount=2			(starts with \t, number of files to send to the client)
//	File=sound/music0.mp3	(filename to send, actual file transferred is ./sound/music0.mp3.gz)
//	File=sound/music1.mp3
//Checksum=abcdef12		
//	FileCount=0			the client is up to date
//Checksum=*			the last entry is always a match all, but the action can be different for all
//	FileCount=-1		disconnect the client (recommended)

int retrieve_config()
{	//retrieve configuration settings for the server
	//update.ini is the configuration file for the server
	char *line_read = (char *) malloc (128);
	int length = 127;
	int current_block = 0;	//stores the index of the current block being worked on
	int current_file = 0;	//stores the index of the current filename/data being retrieved
	int line_number = 0;
	int mode = 0;
		//mode 0 = read general configuration items
		//mode 1 = read checksum blocks
		//mode 2 = read files for a checksum block
	FILE *config;
	if ((config = fopen("update.ini", "r")) == 0)
	{	//error
		printf("Error opening configuration file update.ini\n");
		exit(1);
	}
	
	while (fgets(line_read, length, config) != NULL)
	{	//stop reading when done
		line_number++;
		if (line_read[0] != '#')
		{	//ignore comments
			switch (mode)
			{
				case 0:	//read general configurations
					if (sscanf(line_read, "NumVersions = %d", &number_updates) != 1)
					{
						printf ("Error reading from line number: %d\n", line_number);
						number_updates = 0;
					}
					else
					{
						updates = malloc (number_updates * sizeof(struct update_block));
						//initialize everything to 0
						memset(updates, 0, sizeof(struct update_block) * number_updates);
						printf ("Reading %d blocks of updates\n", number_updates);
						mode = 1;	//start reading a block
					}
					break;
				case 1:
					if (current_block != number_updates)
					{	//dont try to read more than the correct number of update blocks
						if (sscanf(line_read, "Checksum = %x", &(updates[current_block].checksum)) != 1 )
						{
							printf ("Error reading from line number: %d\n", line_number);
						}
						else
						{
							printf("Checksum: %x\n", updates[current_block].checksum);
							mode = 2;	//read number of files
						}
					}
					break;
				case 2:
					if (sscanf(line_read, " FileCount= %d", &(updates[current_block].number_files)) != 1)
					{
						printf ("Error reading from line number: %d\n", line_number);
					}
					else
					{
						updates[current_block].filenames = 
							malloc (sizeof(char*) * updates[current_block].number_files);
						updates[current_block].name_length = 
							malloc (sizeof(int) * updates[current_block].number_files);
						updates[current_block].file_size = 
							malloc (sizeof(unsigned long) * updates[current_block].number_files);
						updates[current_block].new_cs =
							malloc (sizeof(unsigned long) * updates[current_block].number_files);
						printf("Reading %d files for checksum %x\n", 
							updates[current_block].number_files,
							updates[current_block].checksum);
						if (updates[current_block].number_files > 0)
						{
							mode = 3;	//read filenames
						}
						else
						{
							current_block++;
							mode = 1;
						}
					}
					break;
				case 3:
					if (sscanf(line_read, " File= %[^\t\n\r]", line_read) != 1)
					{
						printf ("Error reading from line number: %d\n", line_number);
					}
					else
					{
						updates[current_block].name_length[current_file] = strlen(line_read) + 1;
						updates[current_block].filenames[current_file] = malloc(strlen(line_read) + 1);
						if (updates[current_block].filenames[current_file] == NULL)
						{
							printf("\tFailed to copy filename\n");
						}
						strcpy(updates[current_block].filenames[current_file], line_read);
						printf("Read filename %d: %s\n", current_file, updates[current_block].filenames[current_file]);
						mode = 4;
					}
					break;
				case 4:
					if (sscanf(line_read, " Cs= %x", &(updates[current_block].new_cs[current_file])) != 1)
					{
						printf ("Error reading from line number: %d\n", line_number);
					}
					else
					{
						printf("\tNew Checksum: %x\n", (updates[current_block].new_cs[current_file]));
						current_file++;
						if (current_file == updates[current_block].number_files)
						{
							current_file = 0;
							current_block++;
							mode = 1;
						}
						else
						{
							mode = 3;
						}
					}
				default:
					printf("%s\n", line_read);
					break;
			}
		}
	}
	fclose(config);
	return 0;

}

#if defined _WIN32 || defined __CYGWIN__
DWORD WINAPI handle_client( LPVOID lpParam ) 
{ 
	HANDLE hStdout;
	SOCKET new_fd = (SOCKET) lpParam;
#else
void *handle_client (void* temp_fd)
{
	int new_fd = (int) temp_fd;
#endif

	int x,y;
	char buffer[17];
	unsigned long value;
	memset(buffer, 0, 17);

	//read a dword from the client
	value = get_dword(new_fd);
			
	struct tm *local;
	time_t t;
	t = time(NULL);
	local = localtime(&t);
	printf("Received: %x, %s\n", value, asctime(local));
	
	//check the list for a match (3609e673)
	for (x = 0; x < number_updates; x++)
	{
		if (value == updates[x].checksum)
		{	//proceed with these results
			break;
		}
	}
	if (x == number_updates)
	{	//the last one is a match for everything, so if a match was not found, use the last entry
		printf("Using last table\n");
		x--;	
	}
	
	if (updates[x].number_files < 0)
	{	//disconnect from the client if a negative number of files is to be sent
		#if defined _WIN32 || defined __CYGWIN__
		closesocket(new_fd);
		#else
			close(new_fd);
		#endif
		#if defined _WIN32 || defined __CYGWIN__
			ExitThread(0);
		#endif
		return 0;
	}
	
	send_dword(new_fd, 0xfffffffe);
	
	while (x < number_updates)
	{
		printf("Sending %d files %d\n", updates[x].number_files, x);
		send_dword(new_fd, updates[x].number_files);	//number of files to send?
	
		if (updates[x].number_files > 0)
		{
			printf("Sending block %d\n", x);
			for (y = 0; y < updates[x].number_files; y++)
			{
				printf("Giving file %d: %s\n", y, updates[x].filenames[y]);
				give_file(updates[x].filenames[y], new_fd, updates[x].new_cs[y]);
			}
		}
		x++;
	}
	
//	send_dword(new_fd, 0);	//number of files to send (none)
	
	
	
	send_dword(new_fd, 0x1000000);	//number of servers (backwards order on the bytes)
	//dont write anything if there are no servers
	send_word(new_fd, 0);	//number of players
	
	#if defined _WIN32 || defined __CYGWIN__
		closesocket(new_fd);
	#else
		close(new_fd);
	#endif
	#if defined _WIN32 || defined __CYGWIN__
		ExitThread(0);
	#endif
	return 0; 
} 



int main()
{
	#if defined _WIN32 || defined __CYGWIN__
		WSADATA wsaData;
		SOCKET new_fd = INVALID_SOCKET;
		SOCKET sockfd = INVALID_SOCKET;
		
		if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
		{
			printf("WSAStartup failed!\n");
//			perror("WSAStartup failed!\n");
			return 1;
		}
	#else
		int sockfd;
		int new_fd;  // listen on sock_fd, new connection on new_fd
		
		pthread_t new_thread;
	#endif
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	#if defined _WIN32 || defined __CYGWIN__
		
	#else
		struct sigaction sa;
	#endif
	char yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;	

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; // use my IP
	
	retrieve_config();

	if ((rv = getaddrinfo(NULL, Port, &hints, &servinfo)) != 0) 
	{
		fprintf(stdout, "getaddrinfo: %s\n", gai_strerror(rv));
		#if defined _WIN32 || defined __CYGWIN__
			WSACleanup();
		#endif
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		#if defined _WIN32 || defined __CYGWIN__
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
	  		  	p->ai_protocol)) == INVALID_SOCKET) 
		#else
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
	  		  	p->ai_protocol)) == -1) 
		#endif
		{
			#if defined _WIN32 || defined __CYGWIN__
				WSACleanup();
			#endif
//			perror("server: socket");
			printf("server: socket error\n");
	  		continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
			#if defined _WIN32 || defined __CYGWIN__
				WSACleanup();
			#endif
			freeaddrinfo(servinfo); // all done with this structure		
//			perror("setsockopt");
			printf("setsockopt error\n");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			#if defined _WIN32 || defined __CYGWIN__
				closesocket(sockfd);
				WSACleanup();
			#else
				close(sockfd);
			#endif
//			perror("server: bind");
			printf("server: bind error\n");
			continue;
		}

		break;
	}

	if (p == NULL)  
	{
		fprintf(stdout, "server: failed to bind\n");
		return 2;
	}
	freeaddrinfo(servinfo); // all done with this structure

	#if defined _WIN32 || defined __CYGWIN__
	if (listen(sockfd, BACKLOG) == SOCKET_ERROR) 
	#else
	if (listen(sockfd, BACKLOG) == -1) 
	#endif
	{
//		perror("listen");
		printf("listen error\n");
		exit(1);
	}

	printf("server: waiting for connections...\n");
	fflush (stdout);
	while (1)
	{
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		#if defined _WIN32 || defined __CYGWIN__
		if (new_fd == INVALID_SOCKET)
		#else
		if (new_fd == -1) 
		#endif
		{
//			perror("accept");
			printf("accept error\n");
			continue;
		}
		#if defined _WIN32 || defined __CYGWIN__
		#else
			inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *)&their_addr),
				s, sizeof s);
		#endif
#if defined _WIN32 || defined __CYGWIN__
		HANDLE temp_handle;
		CreateThread( 
			NULL,				   // default security attributes
			0,					  // use default stack size  
			handle_client,	   // thread function name
			(LPVOID) new_fd,		  // argument to thread function 
			0,					  // use default creation flags 
			NULL);   // returns the thread identifier 				
#else
		pthread_create(&new_thread, NULL, handle_client, (void *) new_fd);
#endif
	}
//	printf("Press Enter\n");
//	int status = 4;
//	char *in_buf = (char *) malloc (5);
//	getline(&in_buf, &status, stdin);
	printf("Exiting now...\n");
	return 0;
}
