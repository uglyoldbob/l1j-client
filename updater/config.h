#ifndef _CONFIG_H_
#define _CONFIG_H_

#define MAX_LINE_LENGTH 128
#define MAGIC_ERROR_NUMBER 1723645

#define DEFAULT_PORT "2003"
#define DEFAULT_IP "127.0.0.1"

class config
{
	public:
		config(char* cfile);	//initialize and load all configuration data
		int config_ok();	//1 means success
		char* get_port();
		char* get_addr(int which);
	private:
		int num_errors;
		char* port;
		//int preference;	//used to determine which source to attempt first
		char* domain_name;	//primary source of data
		char* ip_addr;		//secondary source of data
};

#endif