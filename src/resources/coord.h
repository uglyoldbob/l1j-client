#ifndef __COORD_H_
#define __COORD_H_

class pixel_coord;	//the pixel values when the map is rectangular
class map_coord;	//the map coordinates seen by the user
class screen_coord;

class coord
{
	public:
		coord(int tx, int ty);
		~coord();
		
		void print();
		virtual pixel_coord get_pixel() = 0;
		virtual map_coord get_map() = 0;
		virtual screen_coord get_screen() = 0;
	protected:
		int x;
		int y;
};

#endif