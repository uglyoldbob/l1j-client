#include "client.h"
#include "resources/sdl_graphic.h"
#include "sdl_plain_button.h"

sdl_plain_button::sdl_plain_button(int num, int x, int y, sdl_user *who, funcptr *stuff)
	: sdl_button(num, x, y, who, stuff)
{
	if (num != -1)
	{
		client_request t_sdl;
		t_sdl.request = CLIENT_REQUEST_LOAD_SDL_GRAPHIC;
		t_sdl.data.rload.name = 0;
		t_sdl.data.rload.num = num+1;
		t_sdl.data.rload.x = x;
		t_sdl.data.rload.y = y;
		t_sdl.data.rload.type = GRAPH_IMG;
		t_sdl.data.rload.load_type = CLIENT_REQUEST_LOAD_4;
		
		two = new sdl_graphic();
		t_sdl.data.rload.item = two;
		who->add_request(t_sdl);
	}
	else
	{
		two = 0;
	}
}

void sdl_plain_button::draw(SDL_Surface *display)
{
	if (visible == 1)
	{
		if (have_mouse || key_focus)
		{
			if (two != 0)
			{
				two->draw(display);
			}
		}
		else
		{
			if (one != 0)
			{
				one->draw(display);
			}
		}
	}
}


sdl_plain_button::~sdl_plain_button()
{
	if (two != 0)
	{
		delete two;
		two = 0;
	}
}
