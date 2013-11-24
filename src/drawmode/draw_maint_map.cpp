#include "client.h"
#include "draw_maint_map.h"
#include "globals.h"
#include "resources/map_coord.h"
#include "resources/prepared_graphics.h"
#include "resources/screen_coord.h"
#include "resources/tile.h"
#include "sdl_user.h"
#include "widgets/sdl_lin_map.h"
#include "widgets/sdl_text_button.h"
#include "widgets/sdl_widget.h"

draw_maint_map::draw_maint_map(sdl_user *self)
	: sdl_drawmode(self)
{
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");
	
	num_gfx = 0;
	gfx = 0;
	
	num_widgets = 1;
	
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_text_button("Return", 320, 255, owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_ADMIN_MAIN));
	widgets[0]->set_key_focus(true);
	
	themap = new sdl_lin_map(owner, 0, 50, 640, 380);
	map_vis = 0;
	
	mapnum = 69;
	x = 32674;
	y = 32860;
//	mapnum = 4;
//	x = 33068;
//	y = 32806;
	
	//translates to 7fff8000
	//x = 32700;
	//y = 32764;
	
	themap->set_hotspot(mapnum, x, y);
}

/** Handle key press events
 * \bug Crashes when changing modes
 * */
void draw_maint_map::key_press(SDL_KeyboardEvent *button)
{
	sdl_drawmode::key_press(button);
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (button->type == SDL_KEYDOWN)
	{
		switch(button->keysym.sym)
		{
			case SDLK_LEFT:
				y--;
				themap->set_hotspot(mapnum, x, y);
				break;
			case SDLK_RIGHT:
				y++;
				themap->set_hotspot(mapnum, x, y);
				break;
			case SDLK_UP:
				x++;
				themap->set_hotspot(mapnum, x, y);
				break;
			case SDLK_DOWN:
				x--;
				themap->set_hotspot(mapnum, x, y);
				break;
			case SDLK_PAGEUP:
				mapnum++;
				themap->set_hotspot(mapnum, x, y);
				break;
			case SDLK_PAGEDOWN:
				mapnum--;
				themap->set_hotspot(mapnum, x, y);
				break;
			default:
				break;
		}
	}
	SDL_mutexV(draw_mtx);
}

bool draw_maint_map::mouse_leave()
{
	return false;
}

void draw_maint_map::mouse_click(SDL_MouseButtonEvent *here)
{
	if (num_widgets > 0)
	{
		int index = get_widget(here->x, here->y);
		if (index != -1)
		{
			widgets[index]->mouse_click(here);
		}
	}
}

void draw_maint_map::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void draw_maint_map::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void draw_maint_map::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
{	//TODO: handle click and drag movable widgets
	if (num_widgets > 0)
	{
		int from_w = get_widget(from->x, from->y);
		int to_w = get_widget(to->x, to->y);
		if ((from_w != -1) && (to_w != -1))
		{
			if (from_w != to_w)
			{	//only send events if the widgets are different
				mouse_from(from);
				mouse_to(to);
			}
		}
		if ((from_w == -1) && (to_w != -1))
		{
			mouse_to(to);
		}
		if ((from_w != -1) && (to_w == -1))
		{
			mouse_from(from);
		}
	}
}

draw_maint_map::~draw_maint_map()
{
	if (themap != 0)
	{
		delete themap;
		themap = 0;
	}
	SDL_DestroyMutex(draw_mtx);
	draw_mtx = 0;
}

bool draw_maint_map::quit_request()
{
	//owner->stop_thread = true;
	return true;
}

/** This draws a tile at the focused tile on the drawn map
The general method for drawing a tile on a given map coordinate is like this:
The hotspot of the map coordinates map and screen coordinates together
Create a map_coord using the desired map coordinate of the map.
*/
void draw_maint_map::draw_cursor(int x, int y, SDL_Surface *display)
{
	sdl_graphic *left, *right;
	map_coord tempmap(x, y);
	int dx, dy;
	int selal, selbl;
	selal = 2;
	selbl = 89;

	screen_coord thescreen = tempmap.get_screen();

	int master_offsetx = themap->get_offset_x();
	int master_offsety = themap->get_offset_y();

	themap->tile_data[selal].delay_load(selal, owner);

	dx = thescreen.get_x() + master_offsetx;
	dy = thescreen.get_y() + master_offsety;

	left = themap->tile_data[selal].get_tile_left(selbl);
	right = themap->tile_data[selal].get_tile_right(selbl);
	if (left != 0)
		left->drawat(dx, dy, display);
	if (right != 0)
		right->drawat(dx+24, dy, display);
}

void draw_maint_map::draw(SDL_Surface *display)
{
	static Uint32 time_change = SDL_GetTicks();
	static bool draw_the_cursor = false;
	static int halo = 0;
	if ((SDL_GetTicks() + 1000) > time_change)
	{
		draw_the_cursor = true;//!draw_the_cursor;
		if (++halo > 7)
			halo = 0;
		time_change += 50;
	}
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0);
	if (themap != 0)
	{
		themap->draw(display);
		if (draw_the_cursor)
		{	//2, 25
			switch(halo)
			{
				case 0:
					draw_cursor(x-1, y, display);
					break;
				case 1:
					draw_cursor(x-1, y-1, display);
					break;
				case 2:
					draw_cursor(x, y-1, display);
					break;
				case 3:
					draw_cursor(x+1, y-1, display);
					break;
				case 4:
					draw_cursor(x+1, y, display);
					break;
				case 5:
					draw_cursor(x+1, y+1, display);
					break;
				case 6:
					draw_cursor(x, y+1, display);
					break;
				case 7:
					draw_cursor(x-1, y+1, display);
					break;
			}
		}
		themap->draw_info(display, x, y);
	}
	char dispstr[255];
	sprintf(dispstr, "Loading map %d, %d, %d", mapnum, x, y);
	lineage_font.draw(display, 320, 280, dispstr, 0xFFFE);
	sdl_drawmode::draw(display);
	SDL_mutexV(draw_mtx);
}