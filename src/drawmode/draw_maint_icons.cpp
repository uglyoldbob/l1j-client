#include "draw_maint_icons.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_text_button.h"
#include "widgets/sdl_widget.h"

draw_maint_icons::draw_maint_icons(sdl_user *self)
	: sdl_drawmode(self)
{
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");
	icon = 0;

	load_icon(0);
	num_gfx = 0;
	gfx = 0;

	num_widgets = 1;

	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_text_button("Return", 320, 255, owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_ADMIN_MAIN));
	widgets[0]->set_key_focus(true);
}

void draw_maint_icons::load_icon(int i)
{
	background = i;
	char fname[255];
	sprintf(fname, "%d.tbt", background);
	
	if (icon != 0)
	{
		delete icon;
		icon = 0;
	}
	unsigned char *tfile;
	int filesize;
	tfile = (unsigned char*)owner->getfiles->load_file(fname, &filesize, FILE_SPRITEPACK, 0);
	if (tfile == 0)
	{
		printf("Failed to open %s\n", fname);
	}
	else
	{
		icon = new sdl_graphic();
		icon->do_load(0, 0, SDL_RWFromMem(tfile, filesize), 0, GRAPH_ICON);
		delete [] tfile;
		tfile = 0;
	}
}

void draw_maint_icons::key_press(SDL_KeyboardEvent *button)
{
	sdl_drawmode::key_press(button);
	if (button->type == SDL_KEYDOWN)
	{
		switch(button->keysym.sym)
		{
			case SDLK_LEFT:
				if (background > 0)
				{
					background--;
					load_icon(background);
				}
				break;
			case SDLK_RIGHT:
				background++;
				load_icon(background);
				break;
			default:
				break;
		}
	}
}

bool draw_maint_icons::mouse_leave()
{
	return false;
}

draw_maint_icons::~draw_maint_icons()
{
	if (icon != 0)
	{
		delete icon;
		icon = 0;
	}
}

bool draw_maint_icons::quit_request()
{
	//owner->stop_thread = true;
	return true;
}

void draw_maint_icons::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0);
	sdl_drawmode::draw(display);
	char temp[27];
	sprintf(temp, "Displaying %d.tbt", background);
	lineage_font.draw(display, 320, 240, temp, 0x7392);
	if (icon != 0)
	{
		icon->draw(display);
	}
	SDL_mutexV(draw_mtx);
}