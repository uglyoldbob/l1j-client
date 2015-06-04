#include "draw_maint_png.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_text_button.h"
#include "widgets/sdl_widget.h"

draw_maint_png::draw_maint_png(sdl_user *self)
	: sdl_drawmode(self)
{
	background = 0;
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");

	num_gfx = 1;
	gfx = new sdl_graphic*[num_gfx];
	gfx[0] = new sdl_graphic(background, 0, 0, GRAPH_PNG, owner);

	num_widgets = 1;
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_text_button("Return", 320, 255, owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_ADMIN_MAIN));
	extracting = false;
}

void draw_maint_png::key_press(SDL_KeyboardEvent *button)
{
	sdl_drawmode::key_press(button);
	if (button->type == SDL_KEYDOWN)
	{
		switch(button->keysym.sym)
		{
			case SDLK_x:
				if (extracting)
				{
					extracting = false;
				}
				else
				{
					extracting = true;
				}
				background = 0;
				if (gfx[0] != 0)
				{
					delete gfx[0];
					gfx[0] = 0;
				}
				gfx[0] = new sdl_graphic(background, 0, 0, GRAPH_PNG, owner);
				break;
			case SDLK_LEFT:
				if (background > 0)
				{
					background--;
					if (gfx[0] != 0)
					{
						delete gfx[0];
						gfx[0] = 0;
					}
					gfx[0] = new sdl_graphic(background, 0, 0, GRAPH_PNG, owner);
				}
				break;
			case SDLK_RIGHT:
				background++;
				if (gfx[0] != 0)
				{
					delete gfx[0];
					gfx[0] = 0;
				}
				gfx[0] = new sdl_graphic(background, 0, 0, GRAPH_PNG, owner);
				break;
			default:
				break;
		}
	}
}

bool draw_maint_png::mouse_leave()
{
	return false;
}

draw_maint_png::~draw_maint_png()
{
}

bool draw_maint_png::quit_request()
{
	//owner->stop_thread = true;
	return true;
}

void draw_maint_png::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0);
	sdl_drawmode::draw(display);
	char temp[27];
	sprintf(temp, "Displaying %d.png", background);
	lineage_font.draw(display, 320, 240, temp, 0x7392);
	if (extracting && (background < 65535))
	{
		char name[500];
		sprintf(name, "png/%d.bmp", background);
		if (gfx[0]->valid())
			gfx[0]->make_bmp(name);
		
		background++;
		
		if (gfx[0] != 0)
		{
			delete gfx[0];
			gfx[0] = 0;
		}
		gfx[0] = new sdl_graphic(background, 0, 0, GRAPH_PNG, owner);
	}
	else if (extracting && (background == 65535))
	{
		extracting = false;
		background = 0;
		if (gfx[0] != 0)
		{
			delete gfx[0];
			gfx[0] = 0;
		}
		gfx[0] = new sdl_graphic(background, 0, 0, GRAPH_PNG, owner);
	}
	SDL_mutexV(draw_mtx);
}