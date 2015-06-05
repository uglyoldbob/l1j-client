#include "draw_maint_sprites.h"

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
//	owner->game_music.change_music("sound/music0.mp3");

	num_gfx = 0;
	gfx = 0;
	wpn = 0;
	
	num_widgets = 2;

	widgets = new sdl_widget*[num_widgets];
	
	widget_key_focus = 0;
	
	x = 0;
	//dark elf is 2786
	//valakas is 2001
	//2778 is a good blob sprite
	y = 3;
	cur_heading = 0;

	widgets[0] = 0;
	widgets[1] = new sdl_text_button("Return", 310, 420, owner, 
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
	if (button->type == SDL_KEYDOWN)
	{
		switch(button->keysym.sym)
		{
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
			case SDLK_w:
				wpn++;
				if (wpn > 9)
					wpn = 0;
				((sprite*)(widgets[0]))->set_weapon(wpn);
				break;
			case SDLK_s:
				wpn++;
				if (wpn < 0)
					wpn = 9;
				((sprite*)(widgets[0]))->set_weapon(wpn);
				break;
			case SDLK_h:
				((sprite*)(widgets[0]))->hit();
				break;
			case SDLK_r:
				((sprite*)(widgets[0]))->reset_frame();
				break;
			case SDLK_k:
				((sprite*)(widgets[0]))->kill();
				break;
			case SDLK_l:
				((sprite*)(widgets[0]))->attack();
				break;
			case SDLK_q:
				if (cur_heading < 7)
				{
					cur_heading++;
					((sprite*)(widgets[0]))->change_heading(cur_heading);
				}
				else if  (cur_heading == 7)
				{
					cur_heading = 0;
					((sprite*)(widgets[0]))->change_heading(cur_heading);
				}
				break;
			case SDLK_a:
				if (cur_heading > 0)
				{
					cur_heading--;
					redo_sprite();
				}
				else if  (cur_heading == 0)
				{
					cur_heading = 7;
					redo_sprite();
				}
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

void draw_maint_sprites::redo_sprite()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (x < 0)
		x = 0;
	if (x > 8660)
		x = 8660;
	if (y < 0)
		y = 0;
	if (y > 71)
		y= 71;
	if (widgets[0] != 0)
	{
		delete widgets[0];
		widgets[0] = 0;
	}
	widgets[0] = new sprite(320, 400, owner);//new_name, owner);
	((sprite*)(widgets[0]))->load(320, 400, x);
	((sprite*)(widgets[0]))->move(320, 400, cur_heading);
	((sprite*)(widgets[0]))->set_action(y);
	//if (x == 2786)
	//	((sprite*)(widgets[0]))->print_actions();

	SDL_mutexV(draw_mtx);
}

bool draw_maint_sprites::quit_request()
{
	//owner->stop_thread = true;
	return true;
}

void draw_maint_sprites::draw(SDL_Surface *display)
{
	/*	0 = male royal
		1 = female royal
		61 = male knight
		138 = male elf
		
	*/
	static char* heading_table[] = {
		"Facing Up and Left",	
		"Facing Up",
		"Facing Up and Right",
		"Facing Right",
		"Facing Down and Right",
		"Facing Down", 
		"Facing Down and Left",
	 	"Facing Left"};
	static char* action_table[] = {
		"Walking with no weapon", "Attacking with no weapon", "Hit with no weapon", "Standing", 
			"Walking with sword", "Attack with sword", "Getting hit while holding sword", "Standing with sword", "Dying", "9", "10",
		"Walking with weapon 2", "Attacking with weapon 2", "Hit while holding weapon 2", "Standing with weapon 2",
			//axe for royal, claw for dark elf, 
		"Picking up item", "16", "Using wand", "Casting spell", "Using other magic?",
		"Walking with weapon 3", "Attacking with weapon 3", "Hit while holding weapon 3", "Standing with weapon 3",
			//bow and arrow for royal, crossbow for dark elf
		"Walking with weapon 4", "Attacking with weapon 4", "Hit while holding weapon 4", "Standing with weapon 4",
			//poleaxe for royal
		 "28", "29", "Similar to using other magic?",
		"Something weird", "32", "33", "34", "35", "36", "37", "38", "39", 
		"Walking with weapon 2?", "Attacking with weapon 2?", "Hit while holding weapon 2?", "Standing with weapon 2?", "44", "45",
		"Walking with dagger", "Attacking with dagger", "Hit while holding dagger", "Standing with dagger", 
		"Walking with long sword", "Attacking with long sword", "Hit while holding long sword", "Standing with long sword", 
		"Walking with edoryu", "Attacking with edoryu", "Hit while holding edoryu", "Standing with edoryu",
		"Walking with claws", "Attacking with claws", "Hit while holding claws", "Standing with claws", 
		"Walking?", "Unknown motion", "Getting hit", "Standing legs separated", "Standing legs together", "Taunt", "Wave at somebody", "Yay victory", "Stand",
		"Fishing"};
		
	 		
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0x1234);
	char dispstr[255];
	sprintf(dispstr, "Sprite %d-%d heading %d", x, y, cur_heading);
	lineage_font.draw(display, 220, 280, dispstr, 0xFFFE);
	sprintf(dispstr, "Sprite %d        %s        Action %s", x, heading_table[cur_heading], action_table[y]);
	lineage_font.draw(display, 20, 20, dispstr, 0xFFFE);
	sprintf(dispstr, "");
	((sprite*)(widgets[0]))->valid_actions(dispstr);
	lineage_font.draw(display, 20, 30, dispstr, 0xFFFE);
	//display information about current sprite
	/*
		sprite number, action number (name)
		which actions are valid (text form) (and number of frames for each action) + a symbol to indicate all headings for that action do or do not have the same number of frames
			# means all headings have the same number of frames
			! means at least one heading has a different number of frames
			? at least one heading does not exist (has 0 frames)
		
	*/
	sdl_drawmode::draw(display);
	SDL_mutexV(draw_mtx);
}
