#ifndef __GAME_H__
#define __GAME_H__

#include "ecs/system.h"

class Game : System
{
public:
	Game();
	void make_new_game(Thing t); ///< Make a new game with the given entity
private:
	int bob;
};

#endif
