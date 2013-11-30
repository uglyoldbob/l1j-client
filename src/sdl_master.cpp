#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <time.h>

#include "sdl_master.h"
#include "client.h"
#include "resources/pack.h"

/** Initilization of the main gui object */
sdl_master::sdl_master(Uint32 flags)
{
	game_client = new SDL_Thread*[4];
	clients = new sdl_user*[4];
	cdisplay = new SDL_Surface*[4];
	dragging = false;
	for (int i = 0; i < 4; i++)
	{
		game_client[i] = (SDL_Thread*)0;
		clients[i] = (sdl_user*)0;
		cdisplay[i] = (SDL_Surface *)0;
	}
	draw_id = 0;
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
	}
	else
	{
		display = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, flags);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
		if (display == NULL) 
		{
			fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		}
		else
		{
			SDL_WM_SetCaption("Lineage", "Lineage");
			ready = 1;
		}
	}
}

/** Create a game instance, a thread for it, and wait for it to be ready */
void sdl_master::create_client()
{	
	if (game_client[0] == 0)
	{
		cdisplay[0] = SDL_CreateRGBSurface(SDL_SWSURFACE,
							SCREEN_WIDTH, SCREEN_HEIGHT, 16,
							0x7C00, 0x03E0, 0x001F, 0);
		clients[0] = new sdl_user;
		game_client[0] = SDL_CreateThread(run_client, (void*)clients[0]);
		while (!clients[0]->are_you_ready()) {};
	}
}

/** Process gui events and deliver them as required. 
 \todo This should probably limit how frequently it calls draw 
 */
void sdl_master::process_events()
{
	bool done = false;
	
	SDL_MouseMotionEvent mouse_position;
	mouse_position.state = 0;
	mouse_position.x = 0;
	mouse_position.y = 0;
	
	add_draw_timer(50);	//20 fps
	
	SDL_Event event;
	while(!done)
	{
		if (SDL_WaitEvent(&event))
		{ //wait on an event
			switch(event.type)
			{ //Check event type
				case SDL_USEREVENT:
					switch (event.user.code)
					{
						case 0:
							draw();
							break;
						default:
							break;
					}
					break;
				case SDL_MOUSEMOTION:
					mouse_move(&mouse_position, &event.motion);
					mouse_position = event.motion;
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					mouse_click(&event.button);
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					key_press(&event.key);
					break;
				case SDL_QUIT:
					quit_request();
					break;
				case SDL_VIDEORESIZE: //User resized window
					display = SDL_SetVideoMode(event.resize.w, event.resize.h, 16,
						SDL_HWSURFACE | SDL_DOUBLEBUF); // Create new window
					break;
			}
		}
		done = check_users(done);
	}
//	int status;
//	if (game_client[0] != 0)
//		SDL_WaitThread(game_client[0], &status);
}

/** Check to see if any game instances are done and remove them 
 \todo support more than one instance */
bool sdl_master::check_users(bool last)
{
	if (clients[0] != 0)
	{
		if (clients[0]->done)
		{
			printf("Stop sdl_user\n");
			int bla;
			client *tclient = clients[0]->get_client();
			delete clients[0];
			clients[0] = 0;
			tclient->user_is_done();
			tclient = 0;
			SDL_WaitThread(game_client[0], &bla);
			SDL_FreeSurface(cdisplay[0]);
			cdisplay[0] = 0;
			clients[0] = 0;
			game_client[0] = (SDL_Thread*)0;
			last = true;
		}
	}
	return last;
}

Uint32 sdl_master_draw(Uint32 interval, void *parm)
{
	SDL_Event event;
	SDL_UserEvent usrevent;
	usrevent.type = SDL_USEREVENT;
	usrevent.code = 0;
	usrevent.data1 = NULL;
	usrevent.data2 = NULL;
	event.type = SDL_USEREVENT;
	event.user = usrevent;
	SDL_PushEvent(&event);
	return interval;
}

void sdl_master::add_draw_timer(int ms)
{
	do
	{
		draw_id = SDL_AddTimer(ms, sdl_master_draw, 0);
	}
	while (draw_id == 0);
}

/** Draw all the game instances to the screen 
* \todo Support more than one client here */
void sdl_master::draw()
{
	SDL_Rect rect;
	if (clients[0] != 0)
	{
		clients[0]->draw(cdisplay[0]);
		rect.x = 0;
		rect.y = 0;
		rect.w = cdisplay[0]->w;
		rect.h = cdisplay[0]->h;
		SDL_BlitSurface(cdisplay[0], &rect, display, &rect);
		SDL_Flip(display);
	}
	
}

/** The program has been requested to exit for some reason. Ask for permission to close before closing. 
 * \todo Support more than one client here */
bool sdl_master::quit_request()
{
	bool ok_to_quit = true;
	if (clients[0] != 0)
	{
		if (clients[0]->quit_request() == false)
		{
			printf("Client does not want to die\n");
			ok_to_quit = false;
		}
		else
		{
			printf("Client is ready to die\n");
		}
	}
	return ok_to_quit;
}

/** Deliver mouse move events to the proper client 
 \todo Implement mouse dragging somewhere in the mouse_move chain 
 \todo Support more than one client here */
void sdl_master::mouse_move(SDL_MouseMotionEvent *old, SDL_MouseMotionEvent *fresh)
{
	if (clients[0] != 0)
	{
		if ((old->state == SDL_PRESSED) && (fresh->state == SDL_PRESSED))
		{
			//drag event generated here
			dragging = true;
		}
		else
		{
			int old_num = get_client(old->x, old->y);
			if (old_num != get_client(fresh->x, fresh->y))
			{	//pointer tried to move from one client to another
				if (clients[old_num]->mouse_leave())
				{
					printf("Prevent client %d from losing the mouse\n", old_num);
					*fresh = *old;
				}
			}
			int new_num = get_client(fresh->x, fresh->y);
			//don't do an else here, the fresh mouse position may have changed
			if (old_num == get_client(fresh->x, fresh->y))
			{	//mouse moved within a single client
				clients[old_num]->mouse_move(old, fresh);
			}
			else
			{	//mouse was allowed to leave the old client
				clients[old_num]->mouse_from(old);
				clients[new_num]->mouse_to(fresh);
			}
		}
	}
}

/** Deliver mouse clicks to the proper client 
 \todo Implement double click, triple click here */
void sdl_master::mouse_click(SDL_MouseButtonEvent *here)
{
	if ((here->state == SDL_RELEASED) && dragging)
	{
		//drag end event generated here
		dragging = false;
		//fabricate a mouse move event?
	}
	if (clients[0] != 0)
	{
		//TODO: check for double-click by measuring the time since the last click
		int num = get_client(here->x, here->y);
		clients[num]->mouse_click(here);
	}
}

/** Deliver key presses to the proper client 
 * \todo Support more than one client here */
void sdl_master::key_press(SDL_KeyboardEvent *button)
{
	if (clients[0] != 0)
	{
		if (button->type == SDL_KEYDOWN)
		{
			switch(button->keysym.sym)
			{
				case SDLK_PRINT:
					char filename[256];
					time_t rawtime;
					struct tm * timeinfo;
					time ( &rawtime );
					timeinfo = localtime ( &rawtime );
					sprintf(filename, "%s", asctime(timeinfo));
					filename[strlen(filename)-1] = 0;
					strcat(filename, ".bmp");
					for (unsigned int i = 0; i < strlen(filename); i++)
					{
						if ((filename[i] == ' ') || (filename[i] == ':'))
							filename[i] = '_';
					}
					printf("Screenshot to %s\n", filename);
					SDL_SaveBMP(display, filename);
					break;
				default:
					break;
			}
		}
		//TODO: determine which client gets keyboard input
		clients[0]->key_press(button);
	}
}

/** Determine which client owns the given screen coordinate
 * \todo Support more than one client here */
int sdl_master::get_client(int x, int y)
{	//determine which client owns the x/y coordinate specified
	return 0;
}

sdl_master::~sdl_master()
{
	SDL_RemoveTimer(draw_id);
	delete [] game_client;
	game_client = 0;
	delete [] clients;
	clients = 0;
	delete [] cdisplay;
	cdisplay = 0;
}
