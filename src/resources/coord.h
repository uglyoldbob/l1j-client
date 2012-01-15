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
		
		int get_x();
		int get_y();
	protected:
		int x;
		int y;
};

#endif

//map coord +x means
	//+24 screen x
	//-12 screen y
	
//map coord -x means
	//-24 screen x
	//+12 screen y
	
//map coord +y means
	//+24 screen x
	//+12 screen y
	
//map coord -y means
	//-24 screen x
	//-12 screen y


	
//sx = (mx + my) * 24
//sy = (my - mx) * 12