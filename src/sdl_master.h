#ifndef __SDL_MASTER_H_
#define __SDL_MASTER_H_

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#include "sdl_user.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
//#define true 1
//#define false 0
#define COLORKEY 255, 0, 255
//Your Transparent colour

/** 
 This class handles and delivers all gui events to the proper game instance. It allows for multiple game instances to multiple
 servers (they could be the same server or different servers). Currently only one instance is programmed in, but it would not be too
 difficult to add support for others. 1024x768 is not enough resolution for more than one screen of 640x480. Each client has it's
 own thread that runs it which means that anytime a client modifies a structure the gui thread uses, it must use a synchronization
 method (such as one of the mutexes in place). Failure to do so will cause the code to randomly fail as the gui thread accesses
 objects that don't exist (usually a segfault and results in program death).
 */
class sdl_master
{
	public:
		sdl_master(Uint32 flags = SDL_DOUBLEBUF | SDL_HWSURFACE);
		~sdl_master();

		void create_client();	/**< Create a client that will run a game instance*/
		void process_events();	/**< this is the function for the first thread - it handles all gui stuff*/
	private:
		SDL_Surface *display;	/**< This is the entire display, each client gets a part of it */
		SDL_Surface **cdisplay;	/**< The displays that the clients draw on */
		int ready;	/**< Indicates that the program is ready to start doing things */
		sdl_user **clients;	/**< The list of clients (game instances) - multiple games can run at the same time - only one is supported at the moment */
		SDL_Thread **game_client;	/**< array of all game client threads */
		
		void add_draw_timer(int ms);	/**< Adds a timer to draw the screen */
		SDL_TimerID draw_id;	/**< The timer that causes draw events */
		void draw();	/**< Draw the screen */
		
		bool check_users(bool last);	/**< Check to see if any game instances are done */
		int get_client(int x, int y);	/**< Determine which game client owns a screen coordinate */
		void mouse_move(SDL_MouseMotionEvent *old, SDL_MouseMotionEvent *fresh); /**< a mouse move event handler */
		void mouse_click(SDL_MouseButtonEvent *here);	/**< a mouse click event handler */
		void key_press(SDL_KeyboardEvent *button);	/**< A keyboard press event handler */
		bool quit_request();	/**< This handles the request to close the whole program */
		
		bool dragging;
};

#endif
