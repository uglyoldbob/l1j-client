#include "prepared_graphics.h"

prepared_graphics::prepared_graphics()
{
	ready = false;
}

prepared_graphics::~prepared_graphics()
{
	if (pg != 0)
	{
		for (int i = 0; i < num_pg; i++)
		{
			if (pg[i].cleanup)
				delete [] (unsigned short*)pg[i].surf->pixels;
			SDL_FreeSurface(pg[i].surf);
			if (pg[i].position != 0)
				delete pg[i].position;
			if (pg[i].mask != 0)
				delete pg[i].mask;
		}
		delete [] pg;
	}
}

void prepared_graphics::draw(SDL_Surface *display)
{
	if ((pg != 0) && (ready))
	{
		for (int i = 0; i < num_pg; i++)
		{
			SDL_BlitSurface(pg[i].surf, pg[i].mask, display, pg[i].position);
		}
	}
}

bool prepared_graphics::ok()
{
	if ((num_pg > 0) && (pg != 0) && (ready))
	{
		return true;
	}
	else
	{
		return false;
	}
}