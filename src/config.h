#ifndef _CONFIG_H_
#define _CONFIG_H_

#define MAX_LINE_LENGTH 128
#define MAGIC_ERROR_NUMBER 1723645

#define DEFAULT_PORT "2003"
#define DEFAULT_GAME_PORT "2000"
#define DEFAULT_IP "127.0.0.1"

char *get_string(const char* data);

class config
{
	public:
		config(const char* cfile);	//initialize and load all configuration data
		~config();

		int config_ok();	//1 means success
		const char* get_port();
		const char* get_game_port();
		int get_num_names();
		const char* get_addr(int which);
	private:
		int num_errors;
		char* port;
		char* game_port;
		
		char** names;
		int num_names;
};

#endif
