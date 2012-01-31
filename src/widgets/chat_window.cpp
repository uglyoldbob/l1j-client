#include "chat_window.h"
#include "client.h"
#include "globals.h"
#include "widgets/sdl_input_box.h"
#include "widgets/sdl_widget.h"
#include "widgets/text_box.h"

chat_window::chat_window(int x, int y, client *who)
	: sdl_window(1042, x, y, 400, 400, who)
{	//im placed at 124, 363
	//1043.img graphic
	
	num_widgets = 4;
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_widget(1039, 13, 99, who);
	widgets[1] = new sdl_widget(1041, 13, 101, who);
	widgets[2] = new sdl_input_box(12, 30, 102, who);
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

void chat_window::add_line(char *data)
{
	text_box *tb = (text_box*)widgets[3];
	tb->add_line(data);
}

void chat_window::submit_chat(const char *blabla, char type)
{
	myclient->send_packet("ccs", CLIENT_CHAT, type, blabla);
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
				submit_chat(temp->get_str(), 0);
				temp->clear();
			}
				break;
			default:
				widgets[2]->key_press(button);
				break;
		}
	}
}