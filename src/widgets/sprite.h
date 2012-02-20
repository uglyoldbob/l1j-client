#ifndef __SPRITE_H
#define __SPRITE_H

class client;

class sprite : public sdl_widget
{
	public:
		sprite(int x, int y, char *filename, client *who);
		~sprite();
	private:
		sdl_graphic **tiles;
		int num_tiles;
};


#endif

