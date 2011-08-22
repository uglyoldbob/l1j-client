#include "draw_loading.h"

#include "globals.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_animate_button.h"
#include "widgets/sdl_input_box.h"

draw_loading::draw_loading(graphics_data *stuff, sdl_user *self)
	: sdl_drawmode(stuff, self)
{
	owner->game_music.change_music("sound/music0.mp3");
	load_progress = 0;
	load_amount = 0x249f0;	//not sure how this number is generated
	pg = new prepared_graphics;
	pg->num_pg = 2;
	
	int index;
	SDL_Rect *rect;

	pg->pg = new prepared_graphic[2];
	
	pg->pg[0].surf = get_png_image(811, graphx->spritepack);
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
	num_widgets = 0;
	widgets = 0;
	update_load();
}

void draw_loading::set_load_amount(int size)
{
	load_amount = size;
}

void draw_loading::update_load()
{
	float temp = (float)pg->pg[1].surf->w * ((float)load_progress / (float)load_amount);
	pg->pg[1].mask->w = (Uint16)temp;
}

void draw_loading::add_loaded(int size)
{
	load_progress += size;
	update_load();
}

void draw_loading::load_done()
{
	load_progress = load_amount;
	update_load();
	SDL_Delay(250);
	owner->change_drawmode(DRAWMODE_LOGIN);
}

draw_loading::~draw_loading()
{
	if (pg != 0)
		delete pg;
	if (widgets != 0)
		delete [] widgets;
}

bool draw_loading::mouse_leave()
{
	return false;
}

void draw_loading::mouse_click(SDL_MouseButtonEvent *here)
{
}

void draw_loading::mouse_to(SDL_MouseMotionEvent *to)
{
}

void draw_loading::mouse_from(SDL_MouseMotionEvent *from)
{
}

void draw_loading::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
{
}
