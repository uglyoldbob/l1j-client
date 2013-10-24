#ifndef _CONFIG_H_
#define _CONFIG_H_

#define MAX_LINE_LENGTH 128
#define MAGIC_ERROR_NUMBER 1723645

#define DEFAULT_PORT "2003"
#define DEFAULT_GAME_PORT "2000"
#define DEFAULT_IP "127.0.0.1"

char *get_string(const char* data);

struct server_data
{
	char *server_name;	/**< The visible name of the server*/
	char *port;		/**< The port to use for the update server */
	char *game_port;	/**< The port to use for the game server */
	char **names;	/**< The ip address or hostname to use to connect to the server */
	int num_names;	/**< The number of names this server has*/
};

class config
{
	public:
		config(const char* cfile);	//initialize and load all configuration data
		~config();

		int config_ok();	//1 means success
		const char* get_name(int srv);
		const char* get_port(int srv);
		const char* get_game_port(int srv);
		int get_num_names(int srv);
		int get_num_servers();
		const char* get_addr(int srv, int which);
	private:
		int num_errors;
		
		server_data *sdata;
		int num_servers;
};

#endif
