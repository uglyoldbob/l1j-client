#ifndef __COORD_H_
#define __COORD_H_

class pixel_coord;	//the pixel values when the map is rectangular
class map_coord;	//the map coordinates seen by the user
class screen_coord;

/** This class is the base class and is used to convert from one coordinate system to another */
class coord
{
	public:
		coord(int tx, int ty);
		~coord();
		
		void print();
		virtual pixel_coord get_pixel() = 0;	/**< Returns pixel coordinates (pixel_coord)*/
		virtual map_coord get_map() = 0;	/**< Returns map coordinates (map_coord)*/
		virtual screen_coord get_screen() = 0; /**< returns screen coordinated (screen_coord)*/
		
		int get_x();	/**< Returns the x coordinate */
		int get_y();	/**< Returns the x coordinate */
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