#ifndef _SDL_USER_H_
#define _SDL_USER_H_

#include <SDL/SDL.h>

class client;
#include "globals.h"
#include "drawmode/sdl_drawmode.h"
#include "resources/music.h"
class pack;
class prepared_graphics;
class sdl_widget;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
//#define true 1
//#define false 0
#define COLORKEY 255, 0, 255 //Your Transparent colour

void test_func(void * arg);
/**
This class is the gui portion of a game instance. This object belongs to sdl_master. This object has a thread that runs the client portion of the instance. I haven't quite figured out how to consistently determine whether data should be stored here or in the client class so there is bound to be some inconsistencies in the placement depending on how I felt that day.
 */
class sdl_user
{
	public:
		sdl_user(Uint32 flags = SDL_SWSURFACE);
		~sdl_user();
		void quit_client();	/**< Quit the game instance by triggering the client thread through the client object */
		void login(const char *name, const char *pass); /**< login to the game server */
		
		void init_client(client *clnt);	/**< setup the client object as a part of startup */

		void set_login_char(int num, lin_char_info *data);
		void wait_for_mode(enum drawmode wait);	/**< wait for the client to be in the given drawmode */
		bool is_in_mode(enum drawmode here);	/**< is the client in the given drawmode */
		sdl_drawmode *get_drawmode();	/**< Retrieve the drawmode object */
		void wait_ready();	/**< wait until the ready flag is set */
		void change_drawmode(enum drawmode chg);	/**< change to a different drawmode */
		volatile bool done;	/**<used to terminate the client */
		music game_music; /**< the game music object */
		
		friend class sdl_master;
		client *game;	/**< the client object we are attached to */
	private:
		volatile bool ready;	/**< is it ok to draw me */
		volatile enum drawmode draw_mode;	/**< what is the current drawing mode */

		SDL_Surface *display;	/**< the drawing surface assigned to me */
		
		sdl_drawmode *drawmode;	/**< the object that draws everything for me */
		
		SDL_mutex *draw_mtx;	/**< the mutex the gui thread (sdl_master) uses to share data with the client thread */
		
		void draw();	/**< draw */
		
		//for tracking mouse movements 
		void mouse_to(SDL_MouseMotionEvent *to);
		void mouse_from(SDL_MouseMotionEvent *from);
		void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		bool quit_request(); /**< Is it ok for me to quit? */
		bool mouse_leave();	/**< is it ok for the mouse to leave my drawing area? */
				
		void mouse_click(SDL_MouseButtonEvent *here);
		void key_press(SDL_KeyboardEvent *button);
};

#endif
