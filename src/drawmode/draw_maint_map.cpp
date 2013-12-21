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
	
	num_widgets = 2;
	
	widgets = new sdl_widget*[num_widgets];

	themap = new sdl_lin_map(owner, 0, 50, 640, 380);
	widgets[0] = themap;

	widgets[1] = new sdl_text_button("Return", 320, 255, owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_ADMIN_MAIN));
	widgets[1]->set_key_focus(true);

	map_vis = 0;
	
//	mapnum = 69;
//	x = 32674;
//	y = 32860;
	mapnum = 4;
	x = 33068;
	y = 32806;
	
	//translates to 7fff8000
	//x = 32700;
	//y = 32764;
	
	themap->set_hotspot(mapnum, x, y);
}

/** Handle key press events
 *
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

draw_maint_map::~draw_maint_map()
{
	themap = 0;
	SDL_DestroyMutex(draw_mtx);
	draw_mtx = 0;
}

bool draw_maint_map::quit_request()
{
	//owner->stop_thread = true;
	return true;
}

void draw_maint_map::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0);
	sdl_drawmode::draw(display);
	char dispstr[255];
	sprintf(dispstr, "Loading map %d, %d, %d", mapnum, x, y);
	lineage_font.draw(display, 320, 280, dispstr, 0xFFFE);
	SDL_mutexV(draw_mtx);
}