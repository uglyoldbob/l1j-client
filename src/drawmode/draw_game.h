#ifndef __DRAW_GAME_H_
#define __DRAW_GAME_H_

#include "sdl_drawmode.h"
#include "widgets/chat_window.h"
#include "widgets/sdl_lin_map.h"

struct ground_item
{
	char *name;
	uint16_t x, y;
	uint16_t gnd_icon;
	int8_t heading;
	uint8_t emit_light;
	uint32_t count, id;
};

class draw_game : public sdl_drawmode
{
	public:
		draw_game(sdl_user *self);
		~draw_game();
		
		virtual void key_press(SDL_KeyboardEvent *button);

		virtual void draw(SDL_Surface *display);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
		
		virtual bool quit_request();
		void set_selected_char(lin_char_info *me);
		void set_player_id(uint32_t newid);
		void place_character(ground_item *info);
		void remove_character(uint32_t id);
		void set_underwater(int underwater);
		void show_position(chat_window *temp);
		
		void update_hpbar(int cur, int max);
		void update_mpbar(int cur, int max);
		
		void change_map(int map_num);
		void change_location(int nx, int ny);
	private:
		SDL_mutex *draw_mtx;	/**< This is used to prevent the gui thread and the client thread from clashing objects */

		sdl_lin_map *themap;
		int mapnum, x, y;	//the players coordinates

		lin_char_info *character;
		uint32_t charid;
		char *exp_str;
		char *hp_str;
		float hp_width;
		char *mp_str;
		float mp_width;
		char *ac_str;
		char *evil_str;
		char *weight_str;
		char *food_str;
};

#endif