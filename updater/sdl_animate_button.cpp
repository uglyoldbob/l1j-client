#include "sdl_animate_button.h"

int sdl_animate_button::frame_time = 100;

sdl_animate_button::sdl_animate_button(int num, int x, int y, graphics_data *packs, void (*fnctn)(void*), void *ag)
	: sdl_button(-1, x, y, packs, fnctn, ag)
{
	blabla = packs;
	
	in = 0;
	test.img = get_png_image(in, packs->spritepack);
	if (test.img != 0)
	{
		SDL_SetColorKey(test.img, SDL_SRCCOLORKEY, SDL_MapRGB(test.img->format, 0,0,0) );
		test.mask = NULL;
		test.pos = new SDL_Rect;
		test.pos->x = x;
		test.pos->y = y;
	}
		//0 is new char
		//1 - 24 is new char animating
		//animationA, non-highlighted, deleting, highlighted
		//25-86, 87, 88, 89 : female de
		//90-162, 163, 164, 165, : male de
		//166-241, 242, 243, 244 : female elf
		//245-311, 312, 313, 314 : male elf
		//315-374, 375, 376, 377 : female knight
		//378-448, 449, 450, 451 : male knight
		//452-527, 528, 529, 530 : female wizard
		//531-625, 626, 627, 628 : male wizard
		//629-710, 711, 712, 713 : female royal
		//714-798, 799, 800, 801 : male royal
		//841-905, 906, 1128, 907 : male dragon knight
		//908-965, 966, 1129, 967 : female dragon knight
		//969-1036, 1037, 1130, 1038	: male illusionist
		//1039 - 1125, 1039, 1126, 1131, 1127 : female illusionist
}

void sdl_animate_button::draw(SDL_Surface *display)
{
	if (test.img != 0)
	{
		SDL_BlitSurface(test.img, test.mask, display, test.pos);
	}
	if (in != 0)
	{
		char temp[100];
		if (test.img == 0)
		{
			sprintf(temp, "%d.png null", in);
		}
		else
		{
			sprintf(temp, "%d.png", in);
		}
		lineage_font.draw(display, 0, 0, temp, 0xFFFE);
	}
}

void sdl_animate_button::key_press(SDL_KeyboardEvent *button)
{
	switch(button->keysym.sym)
	{
		case SDLK_DOWN:
			if (in > 100)
			{
				in -= 100;
				SDL_FreeSurface(test.img);
				test.img = get_png_image(in, blabla->spritepack);
				if (test.img != 0)
					SDL_SetColorKey(test.img, SDL_SRCCOLORKEY, SDL_MapRGB(test.img->format, 0,0,0) );
			}
			break;
		case SDLK_LEFT:
			if (in > 0)
			{
				in--;
				SDL_FreeSurface(test.img);
				test.img = get_png_image(in, blabla->spritepack);
				if (test.img != 0)
					SDL_SetColorKey(test.img, SDL_SRCCOLORKEY, SDL_MapRGB(test.img->format, 0,0,0) );
			}
			break;
		case SDLK_RIGHT:
			in++;
			SDL_FreeSurface(test.img);
			test.img = get_png_image(in, blabla->spritepack);
			if (test.img != 0)
				SDL_SetColorKey(test.img, SDL_SRCCOLORKEY, SDL_MapRGB(test.img->format, 0,0,0) );
			break;
		case SDLK_UP:
			in += 100;
			SDL_FreeSurface(test.img);
			test.img = get_png_image(in, blabla->spritepack);
			if (test.img != 0)
				SDL_SetColorKey(test.img, SDL_SRCCOLORKEY, SDL_MapRGB(test.img->format, 0,0,0) );
			break;
		default:
			break;
	}
}

void sdl_animate_button::cursor_on()
{
	key_focus = true;
}

void sdl_animate_button::cursor_off()
{
	key_focus = false;
}


sdl_animate_button::~sdl_animate_button()
{
	delete test.pos;
}
