#include "draw_new_char.h"

#include "client.h"
#include "globals.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_animate_button.h"
#include "widgets/sdl_char_info.h"
#include "widgets/sdl_check_button.h"
#include "widgets/sdl_radio_button.h"
#include "widgets/sdl_input_box.h"
#include "widgets/sdl_widget.h"

void dnc_init_new_char(void *arg, void*arg2)
{
	draw_new_char* bob;
	bob = (draw_new_char*)arg;
	int temp;
	temp = int(arg2);
	bob->init_new_char(temp);
}

void dnc_set_gender(void *arg, void*arg2)
{
	draw_new_char* bob;
	bob = (draw_new_char*)arg;
	int temp;
	temp = (int)arg2;
	bob->set_gender(temp);
}

void dnc_stat_up(void* arg1, void* arg2)
{
	draw_new_char* bob;
	bob = (draw_new_char*)arg1;
	int temp;
	temp = (int)arg2;
	bob->stat_up(temp);
}

void dnc_stat_down(void* arg1, void* arg2)
{
	draw_new_char* bob;
	bob = (draw_new_char*)arg1;
	int temp;
	temp = (int)arg2;
	bob->stat_down(temp);
}

void dnc_cancel(void *arg, void *arg2)
{
	draw_new_char* bob;
	bob = (draw_new_char*)arg;
	bob->cancel(arg2);
}

void dnc_submit(void *arg, void* arg2)
{
	draw_new_char* bob;
	bob = (draw_new_char*)arg;
	bob->submit(arg2);
}

draw_new_char::draw_new_char(graphics_data *stuff, sdl_user *self)
	: sdl_drawmode(stuff, self)
{
	pg = new prepared_graphics;
	pg->num_pg = 1;
	pg->pg = new prepared_graphic[1];
	pg->pg[0].surf = get_img(824, graphx->spritepack);
	pg->pg[0].mask = NULL;
	pg->pg[0].position = NULL;
	pg->pg[0].cleanup = true;
	
	pg->ready = true;
	
	num_widgets = 25;
	widgets = new sdl_widget*[num_widgets];
	widget_key_focus = 14;
	widgets[0] = new sdl_plain_button(825, 476, 403, graphx, 
		dnc_submit, this, 0);	//ok
	widgets[1] = new sdl_plain_button(827, 476, 430, graphx, 
		dnc_cancel, this, 0);	//cancel
	widgets[2] = new sdl_plain_button(556, 424, 317, graphx, 
		dnc_stat_down, this, (void*)STAT_STR);	//str down
	widgets[3] = new sdl_plain_button(554, 435, 317, graphx, 
		dnc_stat_up, this, (void*)STAT_STR);	//str up
	widgets[4] = new sdl_plain_button(556, 424, 332, graphx, 
		dnc_stat_down, this, (void*)STAT_DEX);	//dex down
	widgets[5] = new sdl_plain_button(554, 435, 332, graphx, 
		dnc_stat_up, this, (void*)STAT_DEX);	//dex up
	widgets[6] = new sdl_plain_button(556, 424, 347, graphx, 
		dnc_stat_down, this, (void*)STAT_CON);	//con down
	widgets[7] = new sdl_plain_button(554, 435, 347, graphx, 
		dnc_stat_up, this, (void*)STAT_CON);	//con up
	widgets[8] = new sdl_plain_button(556, 498, 317, graphx, 
		dnc_stat_down, this, (void*)STAT_WIS);	//wis down
	widgets[9] = new sdl_plain_button(554, 509, 317, graphx, 
		dnc_stat_up, this, (void*)STAT_WIS);	//wis up
	widgets[10] = new sdl_plain_button(556, 498, 332, graphx, 
		dnc_stat_down, this, (void*)STAT_CHA);//cha down
	widgets[11] = new sdl_plain_button(554, 509, 332, graphx, 
		dnc_stat_up, this, (void*)STAT_CHA);//cha up
	widgets[12] = new sdl_plain_button(556, 498, 347, graphx, 
		dnc_stat_down, this, (void*)STAT_INT);//int down
	widgets[13] = new sdl_plain_button(554, 509, 347, graphx, 
		dnc_stat_up, this, (void*)STAT_INT);//int up
	
	widgets[14] = new sdl_input_box(13, 440, 381, graphx);
	widgets[14]->cursor_on();
	
	widgets[15] = new sdl_radio_button(1753, 332, 11, graphx, 
		dnc_init_new_char, this, (void*)0, &widgets[15], 7, 0);
	widgets[16] = new sdl_radio_button(1755, 542, 11, graphx, 
		dnc_init_new_char, this, (void*)1, &widgets[15], 7, 1);
	widgets[17] = new sdl_radio_button(1757, 332, 67, graphx, 
		dnc_init_new_char, this, (void*)2, &widgets[15], 7, 2);
	widgets[18] = new sdl_radio_button(1759, 542, 67, graphx, 
		dnc_init_new_char, this, (void*)3, &widgets[15], 7, 3);
	widgets[19] = new sdl_radio_button(1761, 332, 118, graphx, 
		dnc_init_new_char, this, (void*)4, &widgets[15], 7, 4);
	widgets[20] = new sdl_radio_button(1749, 542, 118, graphx, 
		dnc_init_new_char, this, (void*)5, &widgets[15], 7, 5);
	widgets[21] = new sdl_radio_button(1751, 332, 166, graphx, 
		dnc_init_new_char, this, (void*)6, &widgets[15], 7, 6);
	
	widgets[22] = new sdl_animate_button(0xf4, 410, 0, graphx, 0, 0, 0);
	
	widgets[23] = new sdl_radio_button(306, 348, 248, graphx, 
		dnc_set_gender, this, (void*)0, &widgets[23], 2, 0);
	widgets[24] = new sdl_radio_button(304, 533, 248, graphx, 
		dnc_set_gender, this, (void*)1, &widgets[23], 2, 1);
	
	sdl_radio_button *bob;
	gender = 0;
	bob = (sdl_radio_button*)widgets[23];
	bob->set();
	bob = (sdl_radio_button*)widgets[16];
	bob->set();
	init_new_char(1);
	reset_stats();
}

draw_new_char::~draw_new_char()
{
}

void draw_new_char::cancel(void *arg)
{
	owner->change_drawmode(DRAWMODE_CHARSEL);
}

void draw_new_char::submit(void *arg)
{
	sdl_animate_button *jim;
	jim = (sdl_animate_button*)widgets[22];
	lin_char_info *bob;
	bob = jim->get_info();
	if (bob->name != 0)
	{
		delete [] bob->name;
	}

	sdl_input_box *george;
	george = (sdl_input_box*)widgets[14];
	bob->name = new char[strlen(george->get_str())+1];
	strcpy(bob->name, george->get_str());
	
	if (strlen(bob->name) > 2)
	{
		if (points_remaining == 0)
		{
			printf("Creating character \"%s\"\n", bob->name);
			owner->game->send_packet("cscccccccc", CLIENT_CREATE_CHAR,
				bob->name, bob->char_type, bob->gender,
				bob->str + add_str, bob->dex + add_dex, bob->con + add_con, 
				bob->wis + add_wis, bob->cha + add_cha, bob->intl + add_int);
			//"scccccccc"
			//name, type, gender,
			//str, dex, con, wis, cha, int
		}
		else
		{
			//message about distributing points
			printf("STUB Distribute all your status points\n");
		}
	}
	else
	{
		//message about the name not being long enough
		printf("STUB Character name is not long enough\n");
	}
}

void draw_new_char::init_new_char(int type)
{
	char_type = type;
	update_char();
}

void draw_new_char::set_gender(int bla)
{
	gender = bla;
	update_char();
}

void draw_new_char::draw(SDL_Surface *display)
{
	sdl_drawmode::draw(display);
	
	sdl_animate_button *jim;
	jim = (sdl_animate_button*)widgets[22];
	lin_char_info *bob;
	bob = jim->get_info();
	
	char temp[5];
	sprintf(temp, "%2d", points_remaining);
	lineage_font.draw(display, 468, 334, temp, 0xFFFE);
	sprintf(temp, "%d", bob->str + add_str);
	lineage_font.draw(display, 406, 317, temp, 0xFFFE);
	sprintf(temp, "%d", bob->dex + add_dex);
	lineage_font.draw(display, 406, 332, temp, 0xFFFE);
	sprintf(temp, "%d", bob->con + add_con);
	lineage_font.draw(display, 406, 347, temp, 0xFFFE);
	sprintf(temp, "%2d", bob->wis + add_wis);
	lineage_font.draw(display, 525, 317, temp, 0xFFFE);
	sprintf(temp, "%2d", bob->cha + add_cha);
	lineage_font.draw(display, 525, 332, temp, 0xFFFE);
	sprintf(temp, "%2d", bob->intl + add_int);
	lineage_font.draw(display, 525, 347, temp, 0xFFFE);
}

void draw_new_char::stat_down(int stat)
{
	switch(stat)
	{
		case STAT_STR:
			if (add_str > 0)
			{
				points_remaining++;
				add_str--;
			}
			break;
		case STAT_CON:
			if (add_con > 0)
			{
				points_remaining++;
				add_con--;
			}
			break;
		case STAT_DEX:
			if (add_dex > 0)
			{
				points_remaining++;
				add_dex--;
			}
			break;
		case STAT_WIS:
			if (add_wis > 0)
			{
				points_remaining++;
				add_wis--;
			}
			break;
		case STAT_CHA:
			if (add_cha > 0)
			{
				points_remaining++;
				add_cha--;
			}
			break;
		case STAT_INT:
			if (add_int > 0)
			{
				points_remaining++;
				add_int--;
			}
			break;
		default:
			break;
	}
}

lin_char_info* draw_new_char::get_char()
{
	sdl_animate_button *jim;
	jim = (sdl_animate_button*)widgets[22];
	lin_char_info *bob;
	bob = jim->get_info();
	return bob;
}

void draw_new_char::stat_up(int stat)
{
	lin_char_info *bob;
	bob = get_char();
	
	if (points_remaining > 0)
	{
		switch(stat)
		{
			case STAT_STR:
				if ((add_str + bob->str) < bob->max_str)
				{
					points_remaining--;
					add_str++;
				}
				break;
			case STAT_CON:
				if ((add_con + bob->con) < bob->max_con)
				{
					points_remaining--;
					add_con++;
				}
				break;
			case STAT_DEX:
				if ((add_dex + bob->dex) < bob->max_dex)
				{
					points_remaining--;
					add_dex++;
				}
				break;
			case STAT_WIS:
				if ((add_wis + bob->wis) < bob->max_wis)
				{
					points_remaining--;
					add_wis++;
				}
				break;
			case STAT_CHA:
				if ((add_cha + bob->cha) < bob->max_cha)
				{
					points_remaining--;
					add_cha++;
				}
				break;
			case STAT_INT:
				if ((add_int + bob->intl) < bob->max_int)
				{
					points_remaining--;
					add_int++;
				}
				break;		
			default:
				break;
		}
	}
}

void draw_new_char::reset_stats()
{
	add_str = 0;
	add_dex = 0;
	add_con = 0;
	add_wis = 0;
	add_cha = 0;
	add_int = 0;
	points_remaining = 75;
	sdl_animate_button *jim;
	jim = (sdl_animate_button*)widgets[22];
	lin_char_info *bob;
	bob = jim->get_info();
	points_remaining -= bob->str;
	points_remaining -= bob->con;
	points_remaining -= bob->dex;
	points_remaining -= bob->cha;
	points_remaining -= bob->wis;
	points_remaining -= bob->intl;
}

void draw_new_char::update_char()
{
	sdl_animate_button *bob;
	delete widgets[22];
	lin_char_info *temp;
	temp = make_lin_char_info(char_type, gender);
	widgets[22] = new sdl_animate_button(0xf4, 410, 0, graphx, 0, 0, 0);
	bob = (sdl_animate_button*)widgets[22];
	bob->set_info(temp);
	bob->animate(true);
	reset_stats();
}

bool draw_new_char::mouse_leave()
{
	return false;
}

void draw_new_char::mouse_click(SDL_MouseButtonEvent *here)
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

void draw_new_char::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void draw_new_char::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void draw_new_char::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
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
