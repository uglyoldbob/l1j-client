#include "chat_window.h"
#include "client.h"
#include "globals.h"
#include "widgets/sdl_input_box.h"
#include "widgets/sdl_widget.h"
#include "widgets/text_box.h"

chat_window::chat_window(int x, int y, sdl_user *who)
	: sdl_window(1042, x, y, 400, 400, who)
{	//im placed at 124, 363
	//1043.img graphic
	
	num_widgets = 4;
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_widget(1039, 13, 99, who);
	widgets[1] = new sdl_widget(1041, 13, 101, who);
	widgets[2] = new sdl_input_box(12, 30, 102, 0, who);
	widgets[3] = new text_box(20, 32, 6*57, 5*12, who);
	
	text_box *tb = (text_box*)widgets[3];
	tb->add_line("This is a test of displaying text");
	tb->add_line("This is another test of displaying text");
	tb->add_line("This is a really, really long example of displaying some text. The number 42 is required to be mentioned during this test.");
	tb->add_line("alsdjfkdlsa;slkdjfkdlsa;slkdfjkdsla;slkdfj");
	tb->add_line("quiweyruiqoweruyqiwoeruiyqweur");
	
	//the widget that allows text entry
	widgets[2]->set_key_focus(true);
	widgets[2]->cursor_on();
	widget_key_focus = 1;
	
	//the scroll bar (1047 - 1050 for body, 1051 for up button, 1053 for down button)
	//the sizing button (1045 for up arrow) (1056 for down arrow)
	//the time of day indicator	
}

chat_window::~chat_window()
{
}

void chat_window::add_line(const char *data)
{
	text_box *tb = (text_box*)widgets[3];
	tb->add_line(data);
}

void chat_window::submit_chat(const char *blabla)
{
	switch (blabla[0])
	{
		case '"':
		{
			char *temp = new char [strlen(blabla) + 1];
			strcpy(temp, blabla);
			char *temp2 = strchr(temp, ' ');
			temp2[0] = 0;
//			myclient->send_packet("css", CLIENT_CHAT_WHISPER, &temp[1], &temp2[1]);
			delete [] temp;
			temp = 0;
		}
			break;
		case '@':
//			myclient->send_packet("ccs", CLIENT_CHAT, 104, &blabla[1]);
			break;
		case '#':
//			myclient->send_packet("ccs", CLIENT_CHAT, 11, &blabla[1]);
			break;
		case '$':
//			myclient->send_packet("ccs", CLIENT_CHAT_GLOBAL, 12, &blabla[1]);
			break;
		case '&':
//			myclient->send_packet("ccs", CLIENT_CHAT_GLOBAL, 3, &blabla[1]);
			break;
		case '%':
//			myclient->send_packet("ccs", CLIENT_CHAT, 13, &blabla[1]);
			break;
		case '*':
//			myclient->send_packet("ccs", CLIENT_CHAT, 14, &blabla[1]);
			break;
		case '/':
		{
			char version[256];
			if (strcmp(&blabla[1], "version") == 0)
			{
				sprintf(version, "VERSION: UNSTABLE BETA compiled on %s", __DATE__);
				add_line(version);
			}
			else
			{
				sprintf(version, "Invalid command \"%s\"", blabla);
				add_line(version);
			}
		}
			break;
		default:
//			myclient->send_packet("ccs", CLIENT_CHAT, 0, blabla);
			break;
	}
	
}

void chat_window::key_press(SDL_KeyboardEvent *button)
{
	if (button->type == SDL_KEYDOWN)
	{
		switch(button->keysym.sym)
		{
			case SDLK_RETURN:
			case SDLK_KP_ENTER:
			{
				sdl_input_box *temp = (sdl_input_box*)widgets[2];
				submit_chat(temp->get_str());
				temp->clear();
			}
				break;
			default:
				widgets[2]->key_press(button);
				break;
		}
	}
}