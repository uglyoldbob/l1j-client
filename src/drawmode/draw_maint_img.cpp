#include "draw_maint_img.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_text_button.h"
#include "widgets/sdl_widget.h"

draw_maint_img::draw_maint_img(graphics_data *stuff, sdl_user *self)
	: sdl_drawmode(stuff, self)
{
	background = 0;
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");
	pg = 0;
	
	num_widgets = 2;
	
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_widget(background, 0, 0, graphx);
	widgets[1] = new sdl_text_button("Return", 320, 255, graphx, 
		(funcptr*)new dam_ptr(this, DRAWMODE_ADMIN_MAIN));
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

bool draw_maint_img::mouse_leave()
{
	return false;
}

void draw_maint_img::mouse_click(SDL_MouseButtonEvent *here)
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

void draw_maint_img::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void draw_maint_img::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void draw_maint_img::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
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

draw_maint_img::~draw_maint_img()
{
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