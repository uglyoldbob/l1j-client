#include <SDL.h>
#include "SDL_image.h"

#include "globals.h"
#include "sdl_user.h"
#include "sdl_button.h"
#include "sdl_widget.h"

sdl_user::sdl_user(Uint32 flags)
{
	load_progress = 0;
	draw_mode = 0;
	graphx = 0;
	pg = 0;
	num_widgets = 0;
	widgets = (sdl_widget**)0;
	ready = false;
	
	load_amount = 0x249f0;	//not sure how this number is generated
	display = SDL_CreateRGBSurface(flags, SCREEN_WIDTH, SCREEN_HEIGHT, 16,
		0x7C00, 0x03E0, 0x001F, 0);
}

sdl_user::~sdl_user()
{
	SDL_FreeSurface(display);
}

int sdl_user::get_widget(int x, int y)
{
	//priority among who gets mouse when there is overlap? 
		//implement a tracking variable and update it using rules
	for (int i = 0; i < num_widgets; i++)
	{
		if (widgets[i]->contains(x,y))
		{
			return i;
		}
	}
	return -1;	//no widget contains that
}

void sdl_user::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void sdl_user::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void sdl_user::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
{	//TODO: handle click and drag movable widgets
	int from_w = get_widget(from->x, from->y);
	int to_w = get_widget(to->x, to->y);
	printf("%d, %d\n", from_w, to_w);
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

void sdl_user::mouse_click(SDL_MouseButtonEvent *here)
{
}

bool sdl_user::mouse_leave()
{
	return false;
}

void sdl_user::give_data(graphics_data *abc)
{
	if (graphx == 0)
	{
		graphx = abc;
		prepare_load1();
		ready = true;
	}
}

void sdl_user::prepare_load1()
{
	if (pg != 0)
		delete pg;
	pg = new prepared_graphics;
	pg->num_pg = 2;
	
	int index;
	SDL_Rect *rect;

	pg->pg = new prepared_graphic[2];
	
	index = getHashIndex("811.png") + 1;
	pg->pg[0].surf = get_png_image("811.png", graphx->spritepack[index]);
	pg->pg[0].mask = NULL;
	pg->pg[0].position = NULL;
	pg->pg[0].cleanup = false;
	
	pg->pg[1].surf = get_img(330, graphx->spritepack);
	rect = new SDL_Rect;
  	rect->x = 0xf1;
  	rect->y = 0x181;
	pg->pg[1].position = rect;
	rect = new SDL_Rect;
	rect->x = 0;
	rect->y = 0;
	rect->h = pg->pg[1].surf->h;
	rect->w = 0;
	pg->pg[1].mask = rect;
	pg->pg[1].cleanup = true;
	pg->ready = true;
}

void sdl_user::add_loaded(int size)
{
	if (draw_mode == 0)
	{
		load_progress += size;
		update_load();
	}
}

void sdl_user::load_done()
{
	if (draw_mode == 0)
	{
		load_progress = load_amount;
		update_load();
		SDL_Delay(250);
		ready = false;
		delete pg;
		pg = 0;
		prepare_login();
		ready = true;
		draw_mode = 1;
	}
}

void sdl_user::update_load()
{
	if (draw_mode == 0)
	{
		float temp = (float)pg->pg[1].surf->w * ((float)load_progress / (float)load_amount);
		pg->pg[1].mask->w = (Uint16)temp;
	}
}

void sdl_user::set_load_amount(int size)
{
	if (draw_mode == 0)
	{
		load_amount = size;
	}
}

void sdl_user::draw()
{
	if (ready)
	{
		pg->draw(display);
		for (int i = 0; i < num_widgets; i++)
		{
			if (widgets[i] != 0)
				widgets[i]->draw(display);
		}
		switch(draw_mode)
		{
			case 0:
				draw_load1();
				break;
			case 1:
				draw_login();
				break;
			default:
				break;
		}
	}
}

void sdl_user::prepare_login()
{
	ready = false;
	pg = new prepared_graphics;
	pg->num_pg = 1;
	pg->pg = new prepared_graphic[1];
	
	int index = getHashIndex("814.png") + 1;
	pg->pg[0].surf = get_png_image("814.png", graphx->spritepack[index]);
	pg->pg[0].mask = NULL;
	pg->pg[0].position = NULL;
	pg->pg[0].cleanup = false;
	pg->ready = true;
	
	num_widgets = 5;
	widgets = new sdl_widget*[num_widgets];
	
	widgets[0] = new sdl_widget(59, 0x1a9, 0x138, graphx);
	widgets[1] = new sdl_button(53, 0x213, 0x183, graphx);
	widgets[2] = new sdl_button(65, 0x213, 0x195, graphx);
	widgets[3] = new sdl_button(55, 0x213, 0x1a8, graphx);
	widgets[4] = new sdl_button(57, 0x213, 0x1c2, graphx);
	
	ready = true;
}

void sdl_user::draw_load1()
{
	
}

void sdl_user::draw_login()
{	//59.img createSubControls, Bitmap	(0x1907000, 0x1a9, 0x138)
	//53.img createSubControls, HighlightButton, LButton (0x213, 0x183, login, 0, 0)
		//54 highlight
	//65.img createSubControls, HighlightButton, LButton (0x213, 0x195, normalMenu, 4, 0)
	//55.img createSubControls, HighlightButton, LButton (0x213, 0x1a8, newAccountMenu, 5, 0)
	//57.img createSubControls, HighlightButton, LButton (0x213, 0x1c2, normalMenu, 6, 0)
	//814.png
	
	
}