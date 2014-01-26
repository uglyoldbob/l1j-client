#ifndef __sprite_H
#define __sprite_H

#include <vector>

#include "sprite_motion.h"

class sprite : public sdl_widget
{
	public:
		sprite(int x, int y, sdl_user *who);
		virtual void draw(SDL_Surface *display);
		virtual void drawat(int x, int y, SDL_Surface *display);
		~sprite();
		void load(int x, int y, int sprnum, int mot_num);
		void move(int x, int y, int heading);
		void change_heading(uint8_t heading);
		void set_motion(int motion);
		screen_coord get_screen();
	private:
		std::vector<sprite_motion *> motions;
		int cur_motion;
		int posx, posy;
		int sprite_num;

		SDL_mutex *delay_mtx;
		bool delay_loading;
		int delay_load_id;

		void expand_motion(int index);	//expands the motion vector to make sure the index will fit
};


#endif

