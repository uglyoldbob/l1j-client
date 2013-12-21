#include "draw_maint_img.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_text_button.h"
#include "widgets/sdl_widget.h"

draw_maint_img::draw_maint_img(sdl_user *self)
	: sdl_drawmode(self)
{
	background = 0;
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");

	num_gfx = 0;
	gfx = 0;

	num_widgets = 2;

	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_widget(background, 0, 0, owner);
	widgets[1] = new sdl_text_button("Return", 320, 255, owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_ADMIN_MAIN));
	widgets[1]->set_key_focus(true);
}

void draw_maint_img::key_press(SDL_KeyboardEvent *button)
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
					if (widgets[0] != 0)
					{
						delete widgets[0];
						widgets[0] = 0;
					}
					widgets[0] = new sdl_widget(background, 0, 0, owner);
				}
				break;
			case SDLK_RIGHT:
				background++;
				if (widgets[0] != 0)
				{
					delete widgets[0];
					widgets[0] = 0;
				}
				widgets[0] = new sdl_widget(background, 0, 0, owner);
				break;
			default:
				break;
		}
	}
}

bool draw_maint_img::mouse_leave()
{
	return false;
}

draw_maint_img::~draw_maint_img()
{
}

bool draw_maint_img::quit_request()
{
	//owner->stop_thread = true;
	return true;
}

void draw_maint_img::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0);
	sdl_drawmode::draw(display);
	char temp[27];
	sprintf(temp, "Displaying %d.img", background);
	lineage_font.draw(display, 320, 240, temp, 0x7392);
	SDL_mutexV(draw_mtx);
}