#ifndef __SPRITE_H
#define __SPRITE_H

class client;

struct sprite_tile
{
	char x;	/**< offset in map coordinates*/
	char y;	/**< offset in mapp coordinates*/
	char h;	/**< unknown*/
	short tile;	/**< reference to the array of tiles on sprite_frame*/
};

struct sprite_frame
{
	short x1;	/**< unknown */
	short y1;	/**< unknown */
	short x2;	/**< unknown */
	short y2;	/**< unknown */
	short num_tiles;	/**< Number of tiles to draw for this frame */
	sprite_tile *tiles;	/**< The array of tiles for the frame */
};

class sprite : public sdl_widget
{
	public:
		sprite(int x, int y, const char *filename, sdl_user *who);
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
};


#endif

