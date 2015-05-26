#include "draw_maint_sfx.h"

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "globals.h"
#include "sdl_user.h"
#include "widgets/sdl_text_button.h"
#include "widgets/sdl_widget.h"

draw_maint_sfx::draw_maint_sfx(sdl_user *self)
	: sdl_drawmode(self)
{
	draw_mtx = SDL_CreateMutex();
	//owner->game_music.change_music("sound/music0.mp3");
	sfx = NULL;
	channel = -1;
	background = 0;

	load_sfx(0);
	num_gfx = 0;
	gfx = 0;

	num_widgets = 1;

	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_text_button("Return", 320, 255, owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_ADMIN_MAIN));
	widgets[0]->set_key_focus(true);
}

void draw_maint_sfx::load_sfx(int i)
{
	owner->game_music.load_sfx(i);
	if (owner->sfx_data[i] != NULL)
	{
		channel = owner->game_music.play_sfx(i, owner);
		if (channel == -1)
		{
			printf("Error playing %d\n", i);
		}
	}
	else
	{
		printf("Cannot play %d\n", i);
	}
}

void draw_maint_sfx::key_press(SDL_KeyboardEvent *button)
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
					load_sfx(background);
				}
				break;
			case SDLK_RIGHT:
				background++;
				load_sfx(background);
				break;
			case SDLK_PAGEUP:
				background += 100;
				load_sfx(background);
				break;
			case SDLK_PAGEDOWN:
				background -= 100;
				if (background < 0)
					background = 0;
				load_sfx(background);
				break;
			case SDLK_p:
				if (owner->sfx_data[background] != NULL)
				{                                              
					channel = owner->game_music.play_sfx(background, owner);
					if (channel == -1)
					{
						printf("Error playing %d\n", background);
					}
				}
				else
				{
					printf("Cannot play %d\n", background);
				}
				break;
			default:
				break;
		}
	}
}

bool draw_maint_sfx::mouse_leave()
{
	return false;
}

draw_maint_sfx::~draw_maint_sfx()
{
	if (sfx != NULL)
	{
		Mix_FreeChunk(sfx);
		sfx = NULL;
	}
}

bool draw_maint_sfx::quit_request()
{
	//owner->stop_thread = true;
	return true;
}

void draw_maint_sfx::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0);
	sdl_drawmode::draw(display);
	char temp[27];
	sprintf(temp, "Sound effect %d.wav", background);
	lineage_font.draw(display, 320, 240, temp, 0x7392);
	SDL_mutexV(draw_mtx);
}