#include "client.h"
#include "draw_game.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/chat_window.h"
#include "widgets/sprite.h"
#include "widgets/sdl_lin_map.h"
#include "widgets/sdl_widget.h"

draw_game::draw_game(sdl_user *self)
	: sdl_drawmode(self)
{
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");
	
	num_gfx = 0;
	gfx = 0;
	character = 0;
	exp_str = new char[20];
	sprintf(exp_str, "%02d      %02.2f", 1, 32.47);
	hp_str = new char[20];
	strcpy(hp_str, "0/0");
	mp_str = new char[20];
	strcpy(mp_str, "0/0");
	ac_str = new char[20];
	strcpy(ac_str, "10");
	evil_str = new char[20];
	strcpy(evil_str, "0");
	weight_str = new char[20];
	strcpy(weight_str, "0%");
	food_str = new char[20];
	strcpy(food_str, "0%");
	
	num_widgets = 6;

	themap = new sdl_lin_map(owner, 0, 0, 640, 380);
	mapnum = 4;

	x = 32549;
	y = 32925;
	themap->set_hotspot(mapnum, x, y);
	
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = themap;
	widgets[1] = new chat_window(124, 363, self);
	widgets[1]->set_key_focus(true);
	widgets[1]->cursor_on();
	widget_key_focus = 1;
	
	widgets[2] = new sdl_widget(1028, 0, 368, self);
		//new sdl_widget(1019, 485, 366, self);
		//new sdl_widget(1028, 0, 368, self);
		//new sdl_widget(1042, 124, 363, self);
		
	widgets[3] = new sdl_widget(1029, 3, 386, self);
	widgets[4] = new sdl_widget(1030, 3, 402, self);
	widgets[5] = new sdl_widget(1031, 3, 423, self);
	//1029 is the exp bar
	//1030 is the regular hp bar
	//1031 is mp bar
		
	//1032 is demon face
	//1033 is swirl
	//1034 is feather
	//1035 is for weight indicator (not overweight)
	//1036 is for overweight
	//1037 is for severe overweight
	//1038 is a mystery bar (possibly food)
	
	hp_width = (float)widgets[4]->getw();
	mp_width = (float)widgets[5]->getw();
	
//	widgets[6] = new sprite(320, 200, owner);//, "2786-8.spr", owner);   //0, 5, 8, 
//	((sprite*)widgets[6])->delay_load(320, 200, "2786-8.spr", owner->get_client());

	//widgets[6] = new sprite(50, 50, "3226-305.spr", owner);
}

void draw_game::change_map(int map_num)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	mapnum = map_num;
	themap->set_hotspot(mapnum, x, y);
	SDL_mutexV(draw_mtx);
}

void draw_game::change_location(int nx, int ny)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	x = nx;
	y = ny;
	themap->set_hotspot(mapnum, x, y);
	SDL_mutexV(draw_mtx);
}

void draw_game::update_hpbar(int cur, int max)
{
	sprintf(hp_str, "%d/%d", cur, max);
	
	float temp = (float)widgets[4]->getw();;
	temp *= (float)cur;
	temp /= (float)max;
	widgets[4]->setmw((int)temp);
}

void draw_game::update_mpbar(int cur, int max)
{
	sprintf(mp_str, "%d/%d", cur, max); 
	
	float temp = (float)widgets[5]->getw() * ((float)cur / (float)max);
	widgets[5]->setmw(temp);
}


void draw_game::key_press(SDL_KeyboardEvent *button)
{
	sdl_drawmode::key_press(button);
}

bool draw_game::mouse_leave()	//is it ok for the mouse to leave?
{
	return false;
}

draw_game::~draw_game()
{
	delete [] exp_str;
	exp_str = 0;
	delete [] hp_str;
	hp_str = 0;
	delete [] mp_str;
	mp_str = 0;
	delete [] ac_str;
	ac_str = 0;
	delete [] evil_str;
	evil_str = 0;
	delete [] weight_str;
	weight_str = 0;
	delete [] food_str;
	food_str = 0;
	if (character != 0)
	{
		if (character->name != 0)
		{
			delete [] character->name;
			character->name = 0;
		}
		if (character->pledge != 0)
		{
			delete [] character->pledge;
			character->pledge = 0;
		}
		delete character;
		character = 0;
	}
	SDL_DestroyMutex(draw_mtx);
	draw_mtx = 0;
}

bool draw_game::quit_request()
{
	return true;
}

//1042.img for chat window?

void draw_game::set_selected_char(lin_char_info *me)
{
	character = new lin_char_info(*me);
	character->name = new char[strlen(me->name)+1];
	strcpy(character->name, me->name);
	character->pledge = new char[strlen(me->pledge)+1];
	strcpy(character->pledge, me->pledge);
	printf("Receiving character %s\n", character->name);
}

void draw_game::set_player_id(uint32_t newid)
{
	charid = newid;
}

void draw_game::set_underwater(int underwater)
{
	printf("STUB Set underwater attribute\n");
}

void draw_game::remove_character(uint32_t id)
{
	themap->remove_character(id);
}

void draw_game::place_character(ground_item *info)
{
	themap->move_sprite(info->id, info->x, info->y, info->gnd_icon);
	if (info->id == charid)
	{
		change_location(info->x, info->y);
	}
	else
	{
	}
}

void draw_game::show_position(chat_window *temp)
{
	char message[50];
	sprintf(message, "location (%d, %d)", x, y);
	temp->add_line(message);
}

void draw_game::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0);
	sdl_drawmode::draw(display);
	owner->smallfont.draw(display, 35, 390, exp_str, 0xFFFE);
	lineage_font.draw(display, 35, 407, hp_str, 0xFFFE);
	lineage_font.draw(display, 35, 426, mp_str, 0xFFFE);
	
	SDL_mutexV(draw_mtx);
}