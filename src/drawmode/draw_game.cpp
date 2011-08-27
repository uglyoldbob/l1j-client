#include "draw_game.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_widget.h"

draw_game::draw_game(graphics_data *stuff, sdl_user *self)
	: sdl_drawmode(stuff, self)
{
	background = 811;
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");
	pg = new prepared_graphics;
	pg->num_pg = 1;
	pg->pg = new prepared_graphic[1];
	
	pg->pg[0].surf = get_png_image(background, graphx->spritepack);
	pg->pg[0].mask = NULL;
	pg->pg[0].position = NULL;
	pg->pg[0].cleanup = false;
	
	pg->ready = true;
	
	num_widgets = 1;
	
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_widget(background, 0, 0, graphx);
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
					widgets[0] = new sdl_widget(background, 0, 0, graphx);
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
				widgets[0] = new sdl_widget(background, 0, 0, graphx);
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