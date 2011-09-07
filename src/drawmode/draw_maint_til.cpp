#include "draw_maint_til.h"
#include "globals.h"
#include "resources/prepared_graphics.h"
#include "resources/tile.h"
#include "sdl_user.h"
#include "widgets/sdl_text_button.h"
#include "widgets/sdl_widget.h"

draw_maint_til::draw_maint_til(graphics_data *stuff, sdl_user *self)
	: sdl_drawmode(stuff, self)
{
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");
	pg = 0;
	
	num_widgets = 1;
	
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_text_button("Return", 320, 255, graphx, change_mode, owner, (void*)DRAWMODE_ADMIN_MAIN);
	widgets[0]->set_key_focus(true);
	
	tileset = new tile;
	background = 23;
	tile_num = 0;
	cur_tile = 0;
	tileset->load(background, graphx->tilepack);
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
				if (background > 1)
				{
					background--;
					if (tileset != 0)
					{
						delete tileset;
					}
					tileset = new tile;
					tileset->load(background, graphx->tilepack);
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
				tileset->load(background, graphx->tilepack);
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
	if (cur_tile != 0)
	{
		delete cur_tile->pos;
		delete cur_tile->mask;
		if (cur_tile->img != 0)
		{
			if (cur_tile->cleanup)
			{
				delete [] (short*)cur_tile->img->pixels;
			}
			SDL_FreeSurface(cur_tile->img);
		}
		delete cur_tile;
		cur_tile = 0;
	}
}

//updates the graphic to reflect the current tile
void draw_maint_til::update_tile()
{
	if (cur_tile != 0)
	{
		delete cur_tile->pos;
		delete cur_tile->mask;
		if (cur_tile->img != 0)
		{
			if (cur_tile->cleanup)
			{
				delete [] (short*)cur_tile->img->pixels;
			}
			SDL_FreeSurface(cur_tile->img);
		}
		delete cur_tile;
		cur_tile = 0;
	}
	cur_tile = tileset->get_tile(tile_num);
}

void draw_maint_til::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0);
	sdl_drawmode::draw(display);
	if (cur_tile != 0)
	{
		SDL_BlitSurface(cur_tile->img, cur_tile->mask, display, cur_tile->pos);
	}
	char temp[27];
	if (tileset != 0)
	{
		sprintf(temp, "Using tileset %d.til, tile %d", background, tile_num);
		//TODO : draw the tile
	}
	else
	{
		sprintf(temp, "Tileset %d.til does not exist", background);
	}
	lineage_font.draw(display, 320, 240, temp, 0x7392);
	SDL_mutexV(draw_mtx);
}