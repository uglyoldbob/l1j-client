#include "client.h"
#include "draw_game.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/chat_window.h"
#include "widgets/sdl_lin_map.h"
#include "widgets/sdl_widget.h"

draw_game::draw_game(sdl_user *self)
	: sdl_drawmode(self)
{
	background = 811;
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");
	pg = 0;
	
	num_widgets = 2;
	
	sdl_lin_map *themap;
	themap = new sdl_lin_map(owner->game->get_tiles(), owner->game, 0, 0, 640, 380);
	themap->set_hotspot(4, 32549, 32925);
	
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = themap;
	widgets[1] = new chat_window(124, 363, self->game);
	widgets[1]->set_key_focus(true);
	widgets[1]->cursor_on();
	widget_key_focus = 1;
		//new sdl_widget(1019, 485, 366, self->game);
		//new sdl_widget(1028, 0, 368, self->game);
		//new sdl_widget(1042, 124, 363, self->game);
}

void draw_game::key_press(SDL_KeyboardEvent *button)
{
	sdl_drawmode::key_press(button);
	if (button->type == SDL_KEYDOWN)
	{
		switch(button->keysym.sym)
		{
			default:
				widgets[1]->key_press(button);
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
	SDL_mutexV(draw_mtx);
}