#ifndef __SDL_WIDGET_H_
#define __SDL_WIDGET_H_

#include <SDL.h>
class client;

#include "globals.h"
#include "resources/sdl_graphic.h"

#include <SDL.h>
class client;

class sdl_widget
{
	public:
		sdl_widget(int x, int y, client *who);
		sdl_widget(int num, int x, int y, client *who);
		virtual ~sdl_widget();
		virtual void draw(SDL_Surface *display);
		
		//for tracking what the mouse is over
		void mouseOn();
		void mouseOff();
		
		virtual void cursor_on();
		virtual void cursor_off();
		
		virtual void set_key_focus(bool arg);
		bool check_key_focus();
		
		virtual void mouse_click(SDL_MouseButtonEvent *here);
		virtual bool contains(int x, int y);	//does this widget contain the given point?
		virtual void key_press(SDL_KeyboardEvent *button);
		void mouse_to(SDL_MouseMotionEvent *to);
		void mouse_from(SDL_MouseMotionEvent *from);
		void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
	protected:
		bool visible;	//determines if the widget should be drawn
		bool key_focus;	//does the widget have keyboard focus?
		bool allow_key_focus;	//am i allowed to have key focus?
		sdl_graphic *one;
		
		bool have_mouse;
		client *myclient;
};

#endif

//these are all img files
//game tips 808
//x 809-810
//help window 811
//up arrow 812 - 813
//down arrow 814 - 815
//check box 817 - 818
//map of all aden? 885, 1294
//general window 883, different color overlay on 884?
//silence cave 886
//calculator 895, buttons 896 - 921
	//or maybe 1069-1098
	//1359-1414

//mini hp/mp bar 930 - 931 (blank, full_hp, full_mp)
//magic window 934 - 968, background + overlays
//x 969-970, scroll 971, down arrow 972-973, up arrow 974-975, scrolldot 976-977

//char window 978-995 ? multiple windows?
//item window 996-1002
	//trash can 1003-1004

//buttons for interface 1005-1018

//Fkeys 1019-1027

//char stat display (lower left) 1028-1038

//chat stuff 1039-1057

//restart quit window 1058-1068

//shop window 1099-1112

//something happening animation 1116-1132

//time of night indicator 1133-1152
//time of day indicator 1213-1246
	//1316-1319
//pet icons 1153-1168
//pet/summon window 1169-1196
//trade window 1197-1212

//hp/mp/exp bars, 1263-1266
//small moving hp/mp bars 1330-1332
//medium moving hp/mp bars 1333-1335
//large moving hp/mp bars 1336-1338

//poison/stone hp bars 1345-1350, 1357-1358

//cooking window 1480-1481
//stat renewal 1731-1747
