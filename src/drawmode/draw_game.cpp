#include "draw_game.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_widget.h"

draw_game::draw_game(sdl_user *self)
	: sdl_drawmode(self)
{
	background = 811;
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");
	pg = 0;
	
	num_widgets = 4;
	
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_widget(background, 0, 0, self->game);
	widgets[1] = new sdl_widget(1019, 485, 366, self->game);
	widgets[2] = new sdl_widget(1028, 0, 368, self->game);
	widgets[3] = new sdl_widget(1042, 124, 363, self->game);
}

void draw_game::key_press(SDL_KeyboardEvent *button)
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
					}
					widgets[0] = new sdl_widget(background, 0, 0, owner->game);
					//SDL_FreeSurface(pg->pg[0].surf);
					//pg->pg[0].surf = get_png_image(background, graphx->spritepack);
				}
				break;
			case SDLK_RIGHT:
				background++;
				if (widgets[0] != 0)
				{
					delete widgets[0];
				}
				widgets[0] = new sdl_widget(background, 0, 0, owner->game);
				//SDL_FreeSurface(pg->pg[0].surf);
				//pg->pg[0].surf = get_png_image(background, graphx->spritepack);
				break;
			default:
				break;
		}
	}
}

void draw_game::mouse_click(SDL_MouseButtonEvent *here)
{
}

void draw_game::mouse_to(SDL_MouseMotionEvent *to)
{
}

void draw_game::mouse_from(SDL_MouseMotionEvent *from)
{
}

void draw_game::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
{
}

bool draw_game::mouse_leave()	//is it ok for the mouse to leave?
{
	return false;
}

draw_game::~draw_game()
{
}

//1042.img for chat window?

void draw_game::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0);
	sdl_drawmode::draw(display);
	char temp[15];
	sprintf(temp, "%d.img", background);
	lineage_font.draw(display, 320, 240, temp, 0x7392);
	SDL_mutexV(draw_mtx);
}