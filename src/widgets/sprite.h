#ifndef __SPRITE_H
#define __SPRITE_H

class client;

struct sprite_tile
{
	char x;
	char y;
	char h;
	short tile;
};

struct sprite_frame
{
	short x1;
	short y1;
	short x2;
	short y2;
	short num_tiles;
	sprite_tile *tiles;
};

class sprite : public sdl_widget
{
	public:
		sprite(int x, int y, char *filename, client *who);
		virtual void draw(SDL_Surface *display);
		~sprite();
	private:
		sdl_graphic **tiles;
		int num_tiles;
		
		int num_frames;
		sprite_frame *frames;
		
		//sequence and frame to draw
		int frame_num;
		
		int loc_x, loc_y;
		Uint32 time_change;
		int **x_mod;
		int **y_mod;
};


#endif

