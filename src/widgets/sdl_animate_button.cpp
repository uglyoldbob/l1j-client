#include "client.h"
#include "sdl_animate_button.h"

int sdl_animate_button::frame_time = 67;

sdl_animate_button::sdl_animate_button(int num, int x, int y, sdl_user *who, funcptr *stuff)
	: sdl_plain_button(-1, x, y, who, stuff)
{
	animating = false;	
	this->x = x;
	this->y = y;
	char_info = 0;
	
	num_anim = 0;
	cur_frame = 0;
		
	set_info(0);
}

lin_char_info *sdl_animate_button::get_info()
{
	return char_info;
}

void sdl_animate_button::set_info(lin_char_info *data)
{	//females get the odd numbers, type is multiplied by 2 and gender is added
	char_info = data;

	delete one;
	delete two;
	delete_animation();
	cur_frame = 0;
	
	int temp;
	if (data != 0)
	{
		temp = (data->char_type * 2) + data->gender;
	}
	else
	{
		temp = -1;
	}
	visible = true;
	switch(temp)
	{
		case 0:	//male royal
			num_anim = 86;
			animates = new sdl_graphic*[num_anim];
			first_anim = 714;
			first_repeat = 0;
			one = new sdl_graphic();//(799, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(801, x, y, myclient, GRAPH_PNG);
			break;
		case 1:	//female royal
			num_anim = 82;
			animates = new sdl_graphic*[num_anim];
			first_anim = 629;
			first_repeat = 0;
			one = new sdl_graphic();//(711, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(713, x, y, myclient, GRAPH_PNG);
			break;
		case 2:	//male knight
			num_anim = 71;
			animates = new sdl_graphic*[num_anim];
			first_anim = 378;
			first_repeat = 0;
			one = new sdl_graphic();//(449, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(451, x, y, myclient, GRAPH_PNG);
			break;
		case 3:	//female knight
			num_anim = 60;
			animates = new sdl_graphic*[num_anim];
			first_anim = 315;
			first_repeat = 0;
			one = new sdl_graphic();//(375, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(377, x, y, myclient, GRAPH_PNG);
			break;
		case 4:	//male elf
			num_anim = 67;
			animates = new sdl_graphic*[num_anim];
			first_anim = 245;
			first_repeat = 0;
			one = new sdl_graphic();//(312, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(314, x, y, myclient, GRAPH_PNG);
			break;
		case 5:	//female elf
			num_anim = 76;
			animates = new sdl_graphic*[num_anim];
			first_anim = 166;
			first_repeat = 0;
			one = new sdl_graphic();//(242, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(244, x, y, myclient, GRAPH_PNG);
			break;
		case 6:	//male wizard
			num_anim = 95;
			animates = new sdl_graphic*[num_anim];
			first_anim = 531;
			first_repeat = 0;
			one = new sdl_graphic();//(626, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(628, x, y, myclient, GRAPH_PNG);
			break;
		case 7:	//female wizard
			num_anim = 76;
			animates = new sdl_graphic*[num_anim];
			first_anim = 452;
			first_repeat = 0;
			one = new sdl_graphic();//(528, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(530, x, y, myclient, GRAPH_PNG);
			break;
		case 8:	//male dark elf
			num_anim = 73;
			animates = new sdl_graphic*[num_anim];
			first_anim = 90;
			first_repeat = 0;
			one = new sdl_graphic();//(163, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(165, x, y, myclient, GRAPH_PNG);
			break;
		case 9: //female dark elf
			num_anim = 62;
			animates = new sdl_graphic*[num_anim];
			first_anim = 25;
			first_repeat = 0;
			one = new sdl_graphic();//(87, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(89, x, y, myclient, GRAPH_PNG);
			break;
		case 10:	//male dragon knight
			num_anim = 65;
			animates = new sdl_graphic*[num_anim];
			first_anim = 841;
			first_repeat = 0;
			one = new sdl_graphic();//(906, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(907, x, y, myclient, GRAPH_PNG);
			break;
		case 11:	//female dragon knight
			num_anim = 58;
			animates = new sdl_graphic*[num_anim];
			first_anim = 908;
			first_repeat = 0;
			one = new sdl_graphic();//(966, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(967, x, y, myclient, GRAPH_PNG);
			break;
		case 12:	//male illusionist
			num_anim = 68;
			animates = new sdl_graphic*[num_anim];
			first_anim = 969;
			first_repeat = 0;
			one = new sdl_graphic();//(1037, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(1038, x, y, myclient, GRAPH_PNG);
			break;
		case 13:	//female illusionist
			num_anim = 87;
			animates = new sdl_graphic*[num_anim];
			first_anim = 1039;
			first_repeat = 0;
			one = new sdl_graphic();//(1126, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(1127, x, y, myclient, GRAPH_PNG);
			break;
		default:	//new character
			num_anim = 24;
			animates = new sdl_graphic*[num_anim];
			first_anim = 1;
			first_repeat = 0;
			one = new sdl_graphic();//(0, x, y, myclient, GRAPH_PNG);
			two = new sdl_graphic();//(0, x, y, myclient, GRAPH_PNG);
			break;
	}
	for (int i = 0; i < num_anim; i++)
		animates[i] = (sdl_graphic*)-1;
}

//0 is new char
//1 - 24 is new char animating
//animationA, non-highlighted, deleting, highlighted
//4		025-086, 087, 088, 089 : female de
//4		090-162, 163, 164, 165 : male de
//2		166-241, 242, 243, 244 : female elf
//2		245-311, 312, 313, 314 : male elf
//1		315-374, 375, 376, 377 : female knight
//1		378-448, 449, 450, 451 : male knight
//3		452-527, 528, 529, 530 : female wizard
//3		531-625, 626, 627, 628 : male wizard
//0		629-710, 711, 712, 713 : female royal
//0		714-798, 799, 800, 801 : male royal
//5		841-905, 906, 1128, 907 : male dragon knight
//5		908-965, 966, 1129, 967 : female dragon knight
//6		969-1036, 1037, 1130, 1038	: male illusionist
//6		1039 - 1125, 1126, 1131, 1127 : female illusionist

void sdl_animate_button::animate(bool start)
{
	if (start)
	{
		cur_frame = 0;
		change_time = SDL_GetTicks() + frame_time;
		animating = true;
	}
	else
	{
		animating = false;
	}
}

void sdl_animate_button::draw(SDL_Surface *display)
{
	if (animating)
	{
		if (change_time <= SDL_GetTicks())
		{
			cur_frame++;
			change_time += frame_time;
			if (cur_frame == num_anim)
				cur_frame = first_repeat;
		}
		//TODO : account for "missing" images on male illusionist
		//TODO : delay image loading time to when image is first displayed
		if (animates[cur_frame] == (sdl_graphic*)-1)
		{
			animates[cur_frame] = new sdl_graphic();//(first_anim+cur_frame, x, y, myclient, GRAPH_PNG);
			SDL_SetAlpha(animates[cur_frame]->get_surf(), SDL_SRCALPHA, 150);
		}
		if (animates[cur_frame] != 0)
			animates[cur_frame]->draw(display);
	}
	else
	{
		sdl_plain_button::draw(display);
	}
}

void sdl_animate_button::key_press(SDL_KeyboardEvent *button)
{
}

void sdl_animate_button::delete_animation()
{
	if (num_anim > 0)
	{
		for (int i = 0; i < num_anim; i++)
		{
			if ((animates[i] != 0) && (animates[i] != (sdl_graphic*)-1))
			{
				delete animates[i];
			}
		}
		delete [] animates;
	}
}

sdl_animate_button::~sdl_animate_button()
{
	delete_animation();
}
