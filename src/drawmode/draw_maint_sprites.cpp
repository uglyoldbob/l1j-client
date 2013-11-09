#include "draw_maint_sprites.h"

#include "client.h"
#include "globals.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_animate_button.h"
#include "widgets/sdl_input_box.h"
#include "widgets/sdl_text_button.h"
#include "widgets/sprite.h"
#include "widgets/text_box.h"

load_ptr::load_ptr(draw_maint_sprites *bla, int val)
{
	ref = bla;
	which = val;
}

void load_ptr::go()
{
	
}

draw_maint_sprites::draw_maint_sprites(sdl_user *self)
	: sdl_drawmode(self)
{
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");

	num_gfx = 0;
	gfx = 0;
	
	num_widgets = 2;

	widgets = new sdl_widget*[num_widgets];
	
	widget_key_focus = 0;
	
	x = 2786;	//dark elf is 2786
	//2778 is a good blob sprite
	y = 8;

	widgets[0] = 0;
	widgets[1] = new sdl_text_button("Return", 320, 410, owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_ADMIN_MAIN));
	widgets[1]->set_key_focus(true);
	redo_sprite();
	//widgets[num_servers+1] = new sprite(50, 50, "6258-0.spr", owner);
		//6256-173 nothing?
		//6256-181 nothing?
}

draw_maint_sprites::~draw_maint_sprites()
{

}

void draw_maint_sprites::key_press(SDL_KeyboardEvent *button)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	sdl_drawmode::key_press(button);
	sprite *temp = (sprite*)widgets[0];
	if (button->type == SDL_KEYDOWN)
	{
		switch(button->keysym.sym)
		{
			case SDLK_LEFT:
				y--;
				redo_sprite();
				break;
			case SDLK_RIGHT:
				y++;
				redo_sprite();
				break;
			case SDLK_UP:
				x++;
				redo_sprite();
				break;
			case SDLK_DOWN:
				x--;
				redo_sprite();
				break;
			case SDLK_PAGEUP:
				x += 100;
				redo_sprite();
				break;
			case SDLK_PAGEDOWN:
				x -= 100;
				if (x < 0)
					x = 0;
				redo_sprite();
				break;
			default:
				break;
		}
	}
	SDL_mutexV(draw_mtx);
}

bool draw_maint_sprites::mouse_leave()
{
	return false;
}

void draw_maint_sprites::mouse_click(SDL_MouseButtonEvent *here)
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

void draw_maint_sprites::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void draw_maint_sprites::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void draw_maint_sprites::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
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

void draw_maint_sprites::redo_sprite()
{
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (widgets[0] != 0)
	{
		delete widgets[0];
		widgets[0] = 0;
	}
	char new_name[50];
	memset(new_name, 0, 50);
	sprintf(new_name, "%d-%d.spr", x, y);
	widgets[0] = new sprite(320, 400, new_name, owner);
}

bool draw_maint_sprites::quit_request()
{
	//owner->stop_thread = true;
	return true;
}

void draw_maint_sprites::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0x1234);
	char dispstr[255];
	sprintf(dispstr, "Sprite %d-%d", x, y);
	lineage_font.draw(display, 220, 280, dispstr, 0xFFFE);
	sdl_drawmode::draw(display);
	SDL_mutexV(draw_mtx);
}