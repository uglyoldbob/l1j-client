#include "client.h"
#include "draw_maint_til.h"
#include "globals.h"
#include "resources/prepared_graphics.h"
#include "resources/tile.h"
#include "sdl_user.h"
#include "widgets/sdl_text_button.h"
#include "widgets/sdl_widget.h"

draw_maint_til::draw_maint_til(sdl_user *self)
	: sdl_drawmode(self)
{
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");
	
	num_gfx = 0;
	gfx = 0;
	
	num_widgets = 1;
	
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_text_button("Return", 320, 255, owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_ADMIN_MAIN));
	widgets[0]->set_key_focus(true);
	
	tileset = new tile;
	background = 23;
	tile_num = 0;
	cur_tile = 0;

	client_request t_sdl;
	t_sdl.request = CLIENT_REQUEST_LOAD_TILE;
	t_sdl.data.tload.which = background;
	t_sdl.data.tload.item = tileset;
	self->add_request(t_sdl);
	update_tile();
}

void draw_maint_til::key_press(SDL_KeyboardEvent *button)
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
					if (tileset != 0)
					{
						delete tileset;
					}
					tileset = new tile;
					client_request t_sdl;
					t_sdl.request = CLIENT_REQUEST_LOAD_TILE;
					t_sdl.data.tload.which = background;
					t_sdl.data.tload.item = tileset;
					owner->add_request(t_sdl);
					update_tile();
				}
				break;
			case SDLK_RIGHT:
				background++;
				if (tileset != 0)
				{
					delete tileset;
				}
				tileset = new tile;
				client_request t_sdl;
				t_sdl.request = CLIENT_REQUEST_LOAD_TILE;
				t_sdl.data.tload.which = background;
				t_sdl.data.tload.item = tileset;
				owner->add_request(t_sdl);
				update_tile();
				break;
			case SDLK_UP:
				if ((tile_num + 1) < tileset->get_amnt())
				{
					tile_num++;
					update_tile();
				}
				break;
			case SDLK_DOWN:
				if (tile_num > 0)
				{
					tile_num--;
					update_tile();
				}
				break;
			default:
				break;
		}
	}
}

bool draw_maint_til::quit_request()
{
	//owner->stop_thread = true;
	return true;
}

bool draw_maint_til::mouse_leave()
{
	return false;
}

void draw_maint_til::mouse_click(SDL_MouseButtonEvent *here)
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

void draw_maint_til::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void draw_maint_til::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void draw_maint_til::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
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

draw_maint_til::~draw_maint_til()
{
	delete tileset;
}

//updates the graphic to reflect the current tile
void draw_maint_til::update_tile()
{
	cur_tile = tileset->get_tile_left(tile_num);
}

void draw_maint_til::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0);
	sdl_drawmode::draw(display);
	char temp[50];
	memset(temp, 0, 50);
	if (tileset != 0)
	{
		if (cur_tile != 0)
		{
			cur_tile->draw(display);
			sprintf(temp, "Using tileset %d.til, tile %d", background, tile_num);
		}
		else
		{
			update_tile();
			sprintf(temp, "Tileset %d.til, tile %d invalid?", background, tile_num);
		}
	}
	else
	{
		sprintf(temp, "Tileset %d.til does not exist", background);
	}
	lineage_font.draw(display, 320, 240, temp, 0x7392);
	SDL_mutexV(draw_mtx);
}