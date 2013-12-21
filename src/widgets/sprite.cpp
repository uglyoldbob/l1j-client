#include "sdl_user.h"
#include "sprite.h"

sprite::sprite(int x, int y, sdl_user *who)
	: sdl_widget(x, y, who)
{
	delay_mtx = SDL_CreateMutex();
	delay_loading = false;
	cur_motion = -1;
}

void sprite::set_motion(int motion)
{
	
}

void sprite::load(int x, int y, int sprnum, int mot_num)
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	int work_me;
	sprite_num = sprnum;
	if (mot_num == -1)
	{
		work_me = 8;
	}
	else
	{
		work_me = mot_num;
	}
	expand_motion(work_me);
	if (motions[work_me] == 0)
	{
		cur_motion = work_me;
		motions[work_me] = new sprite_motion(x, y, myclient);
		char temp[25];
		sprintf(temp, "%d-%d.spr", sprnum, work_me);
		motions[work_me]->delay_load(x, y, temp);
		motions[work_me]->move(x, y);
	}
	SDL_mutexV(delay_mtx);
}

void sprite::expand_motion(int index)
{
	if (motions.size() <= index)
	{
		motions.resize(index+1, 0);
	}
}

//arg2 + frames[frame_num].tiles[i].x - mapX
//arg3 + frames[frame_num].tiles[i].y - mapY
//tile
//h

screen_coord sprite::get_screen()
{
	map_coord temp(0, 0);
	if (motions[cur_motion] != 0)
	{
		return motions[cur_motion]->get_screen();
	}
	else
	{
		return temp.get_screen();
	}
}

void sprite::draw(SDL_Surface *display)
{
	drawat(400, 300, display);
}

void sprite::move(int x, int y, int heading)
{
	if (heading != cur_motion)
	{
		expand_motion(heading);
		cur_motion = heading;
		motions[cur_motion] = new sprite_motion(x, y, myclient);
		char temp[25];
		sprintf(temp, "%d-%d.spr", sprite_num, cur_motion);
		motions[cur_motion]->delay_load(x, y, temp);
		motions[cur_motion]->move(x, y);
	}
	if (motions[cur_motion] != 0)
	{
		motions[cur_motion]->move(x, y);
	}
}

void sprite::drawat(int x, int y, SDL_Surface *display)
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	if (cur_motion != -1)
	{
		if (motions[cur_motion] != 0)
			motions[cur_motion]->drawat(x, y, display);
	}
	SDL_mutexV(delay_mtx);
}

sprite::~sprite()
{
	for (int i = 0; i < motions.size(); i++)
	{
		if (motions[i] != 0)
		{
			delete motions[i];
			motions[i] = 0;
		}
	}
	SDL_DestroyMutex(delay_mtx);
}