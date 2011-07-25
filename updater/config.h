#ifndef _CONFIG_H_
#define _CONFIG_H_

#define MAX_LINE_LENGTH 128
#define MAGIC_ERROR_NUMBER 1723645

#define DEFAULT_PORT "2003"
#define DEFAULT_GAME_PORT "2000"
#define DEFAULT_IP "127.0.0.1"

class config
{
	public:
		config(const char* cfile);	//initialize and load all configuration data
		~config();
		int config_ok();	//1 means success
		const char* get_port();
		const char* get_addr(int which);
	private:
		int num_errors;
		char* port;
		char* game_port;
		//int preference;	//used to determine which source to attempt first
		//TODO: change from a single domain and single ip address to an array of hostname/ipaddresses
		char* domain_name;	//primary source of data
		char* ip_addr;		//secondary source of data
};

#endif
