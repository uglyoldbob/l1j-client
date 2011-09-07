#include "sdl_animate_button.h"

int sdl_animate_button::frame_time = 67;

sdl_graphic *make_sdl_graphic_png(int num, int x, int y, graphics_data *packs)
{
	sdl_graphic *ret;
	if (num != -1)
	{
		ret = new sdl_graphic;
		ret->img = get_png_image(num, packs->spritepack);
		if (ret->img != 0)
		{
			SDL_SetColorKey(ret->img, SDL_SRCCOLORKEY, SDL_MapRGB(ret->img->format, 0, 0, 0));
			ret->pos = make_sdl_rect(x, y, ret->img->w, ret->img->h);
			ret->mask = make_sdl_rect(0, 0, ret->img->w, ret->img->h);
			ret->cleanup = false;
		}
		else
		{
			delete ret;
			ret = 0;
		}
	}
	else
	{
		ret = 0;
	}
	return ret;
}

void delete_sdl_graphic_png(sdl_graphic *stuff)
{
	if ((stuff != 0) && (stuff != (sdl_graphic*)-1))
	{
		delete stuff->pos;
		delete stuff->mask;
		if (stuff->img != 0)
		{
			SDL_FreeSurface(stuff->img);
		}
		delete stuff;
		stuff = 0;
	}
}

sdl_animate_button::sdl_animate_button(int num, int x, int y, graphics_data *packs, 
	void (*fnctn)(void*, void*), void *ag, void* ag2)
	: sdl_plain_button(-1, x, y, packs, fnctn, ag, ag2)
{
	blabla = packs;
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

	delete_sdl_graphic_png(one);
	delete_sdl_graphic_png(two);
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
			one = make_sdl_graphic_png(799, x, y, blabla);
			two = make_sdl_graphic_png(801, x, y, blabla);
			break;
		case 1:	//female royal
			num_anim = 82;
			animates = new sdl_graphic*[num_anim];
			first_anim = 629;
			first_repeat = 0;
			one = make_sdl_graphic_png(711, x, y, blabla);
			two = make_sdl_graphic_png(713, x, y, blabla);
			break;
		case 2:	//male knight
			num_anim = 71;
			animates = new sdl_graphic*[num_anim];
			first_anim = 378;
			first_repeat = 0;
			one = make_sdl_graphic_png(449, x, y, blabla);
			two = make_sdl_graphic_png(451, x, y, blabla);
			break;
		case 3:	//female knight
			num_anim = 60;
			animates = new sdl_graphic*[num_anim];
			first_anim = 315;
			first_repeat = 0;
			one = make_sdl_graphic_png(375, x, y, blabla);
			two = make_sdl_graphic_png(377, x, y, blabla);
			break;
		case 4:	//male elf
			num_anim = 67;
			animates = new sdl_graphic*[num_anim];
			first_anim = 245;
			first_repeat = 0;
			one = make_sdl_graphic_png(312, x, y, blabla);
			two = make_sdl_graphic_png(314, x, y, blabla);
			break;
		case 5:	//female elf
			num_anim = 76;
			animates = new sdl_graphic*[num_anim];
			first_anim = 166;
			first_repeat = 0;
			one = make_sdl_graphic_png(242, x, y, blabla);
			two = make_sdl_graphic_png(244, x, y, blabla);
			break;
		case 6:	//male wizard
			num_anim = 95;
			animates = new sdl_graphic*[num_anim];
			first_anim = 531;
			first_repeat = 0;
			one = make_sdl_graphic_png(626, x, y, blabla);
			two = make_sdl_graphic_png(628, x, y, blabla);
			break;
		case 7:	//female wizard
			num_anim = 76;
			animates = new sdl_graphic*[num_anim];
			first_anim = 452;
			first_repeat = 0;
			one = make_sdl_graphic_png(528, x, y, blabla);
			two = make_sdl_graphic_png(530, x, y, blabla);
			break;
		case 8:	//male dark elf
			num_anim = 73;
			animates = new sdl_graphic*[num_anim];
			first_anim = 90;
			first_repeat = 0;
			one = make_sdl_graphic_png(163, x, y, blabla);
			two = make_sdl_graphic_png(165, x, y, blabla);
			break;
		case 9: //female dark elf
			num_anim = 62;
			animates = new sdl_graphic*[num_anim];
			first_anim = 25;
			first_repeat = 0;
			one = make_sdl_graphic_png(87, x, y, blabla);
			two = make_sdl_graphic_png(89, x, y, blabla);
			break;
		case 10:	//male dragon knight
			num_anim = 65;
			animates = new sdl_graphic*[num_anim];
			first_anim = 841;
			first_repeat = 0;
			one = make_sdl_graphic_png(906, x, y, blabla);
			two = make_sdl_graphic_png(907, x, y, blabla);
			break;
		case 11:	//female dragon knight
			num_anim = 58;
			animates = new sdl_graphic*[num_anim];
			first_anim = 908;
			first_repeat = 0;
			one = make_sdl_graphic_png(966, x, y, blabla);
			two = make_sdl_graphic_png(967, x, y, blabla);
			break;
		case 12:	//male illusionist
			num_anim = 68;
			animates = new sdl_graphic*[num_anim];
			first_anim = 969;
			first_repeat = 0;
			one = make_sdl_graphic_png(1037, x, y, blabla);
			two = make_sdl_graphic_png(1038, x, y, blabla);
			break;
		case 13:	//female illusionist
			num_anim = 87;
			animates = new sdl_graphic*[num_anim];
			first_anim = 1039;
			first_repeat = 0;
			one = make_sdl_graphic_png(1126, x, y, blabla);
			two = make_sdl_graphic_png(1127, x, y, blabla);
			break;
		default:	//new character
			num_anim = 24;
			animates = new sdl_graphic*[num_anim];
			first_anim = 1;
			first_repeat = 0;
			one = make_sdl_graphic_png(0, x, y, blabla);
			two = make_sdl_graphic_png(0, x, y, blabla);
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
		//TODO : account for missing images on male illusionist
		//TODO : delay image loading time to when image is first displayed
		if (animates[cur_frame] == (sdl_graphic*)-1)
			animates[cur_frame] = make_sdl_graphic_png(first_anim+cur_frame, x, y, blabla);
		if (animates[cur_frame] != 0)
			SDL_BlitSurface(animates[cur_frame]->img, animates[cur_frame]->mask, display, animates[cur_frame]->pos);
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
				delete_sdl_graphic_png(animates[i]);
			}
		}
		delete animates;
	}
}

sdl_animate_button::~sdl_animate_button()
{
	delete_animation();
}
