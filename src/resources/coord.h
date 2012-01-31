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

//sx = 24mx + 24my
//sy = 12my - 12mx

//  sx = 24mx + 24my
//-2sy = 24mx - 24my

//sx + 2sy = 48my

//sx - 2sy = 48mx
//mx = (sx - 2sy) / 48

//my = (sx + 2sy)/48

//(sx/24) + (sy/12) = 2*my

//sx/12 + sy/6 = my

//(sx/12 - sy/6) = mx