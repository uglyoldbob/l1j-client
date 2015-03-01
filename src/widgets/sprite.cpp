#include "resources/global_resource_template.h"
#include "sdl_user.h"
#include "sprite.h"

server_resource<sprite_motion> sprite::sprite_gfx;

sprite::sprite(int x, int y, sdl_user *who)
	: sdl_widget(x, y, who)
{
	delay_mtx = SDL_CreateMutex();
	delay_loading = false;
	cur_action = -1;
}

void sprite::load_sprite_gfx(unsigned int id)
{
	//check for and load generic resource
	//check for and load server-specific resource
	base_resource<sprite_motion> generic_gfx;
	if (generic_gfx[id].is_loaded() == 0)
	{
		char filename[100];
		sprintf(filename, "%d-%d.spr", (id>>16)&0xFFFF, id&0xFFFF);
		//printf("Loading %s\n", filename);
		generic_gfx[id].load(0, 0, filename, myclient);
	}
}

void sprite::load_generic_sprite_data()
{
	base_resource<sprite_definition> *sprite_defs = new base_resource<sprite_definition>;
	global_files *getfiles = new global_files();
	unsigned char *sprite_data;
	int spr_num = 0;
	int ani_num;
	int sprite_dlength;
	sprite_data = (unsigned char*)getfiles->load_file("sprite_table.txt", &sprite_dlength, FILE_REGULAR2, 0);
	for (int i = 0; i < sprite_dlength;i++)
	{
		if (sprite_data[i] == 0xd)
			sprite_data[i] = ' ';
	}
	if (sprite_data == 0)
	{
		printf("sprite_table.txt not found\n");
	}
	else
	{
		int offset = 1;
		int sprnum = 0;
		while (isspace(sprite_data[offset])) { offset++; }
		//this number is the number of sequences for the sprite
		ani_num = atoi((char*)&sprite_data[offset]);
		//printf("The number is %d (sprite number 0)\n", ani_num);
		while (isdigit(sprite_data[offset])) { offset++; }
		if (sprite_data[offset] == '=')
		{
			offset += 1;
			int alias = atoi((char*)&sprite_data[offset]);
			//alias replaces? the number used for animations
//			printf("Alias of %d found\n", alias);
			ani_num = alias;
			while (isdigit(sprite_data[offset])) { offset++; }
			while (!isdigit(sprite_data[offset])) { offset++; }
		}
		while (offset < sprite_dlength)
		{
		while (isspace(sprite_data[offset])) { offset++; }
		if (sprite_data[offset] == 0) 
		{
			offset++;
			spr_num++;
			//printf("'%.15s' ", &sprite_data[offset]);
			ani_num = atoi((char*)&sprite_data[offset]);
			//printf("The number is %d (sprite number %d)\n", ani_num, spr_num);
			while (isdigit(sprite_data[offset])) { offset++; }
			if (sprite_data[offset] == '=')
			{
				offset += 1;
				int alias = atoi((char*)&sprite_data[offset]);
				ani_num = alias;
//				printf("\tAlias of %d found\n", alias);
				while (isdigit(sprite_data[offset])) { offset++; }
				while (!isdigit(sprite_data[offset])) { offset++; }
			}
		}
		while (isspace(sprite_data[offset])) { offset++; }
		if (sprite_data[offset] == 0xd2)
		{
			offset++;
		}
		else
		{
		//printf("\t'%.10s' ", &sprite_data[offset]);
		int value = atoi((char*)&sprite_data[offset]);
		while (isdigit(sprite_data[offset])) { offset++; }
		int temp;
		//printf("\tmode=%d\n", value);
		switch (value)
		{
			case 100:	//switches
				//read a value, then read 4 times that many values
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
				for (int i = 0; i < temp; i++)
				{
					while (isspace(sprite_data[offset])) { offset++; }
					atoi((char*)&sprite_data[offset]);
					if (sprite_data[offset]=='-'){offset++;}
					while (isdigit(sprite_data[offset])) { offset++; }
					while (isspace(sprite_data[offset])) { offset++; }
					atoi((char*)&sprite_data[offset]);
					if (sprite_data[offset]=='-'){offset++;}
					while (isdigit(sprite_data[offset])) { offset++; }
					while (isspace(sprite_data[offset])) { offset++; }
					atoi((char*)&sprite_data[offset]);
					if (sprite_data[offset]=='-'){offset++;}
					while (isdigit(sprite_data[offset])) { offset++; }
					while (isspace(sprite_data[offset])) { offset++; }
					atoi((char*)&sprite_data[offset]);
					if (sprite_data[offset]=='-'){offset++;}
					while (isdigit(sprite_data[offset])) { offset++; }
				}
				break;
			case 101: //shadow
				//read one value
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			case 102: //objType
				//read one value
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			case 103: //altAttack
				//read one value
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			case 104: //attr
				//read one value
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			case 105: //clothing
				//read a value
				//read 0 or that value of numbers, whichever is higher
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				for (int i = 0; i < temp; i++)
				{
					while (isspace(sprite_data[offset])) { offset++; }
					int temp2 =atoi((char*)&sprite_data[offset]);
					while (isdigit(sprite_data[offset])) { offset++; }
//					printf("\t\t\t%d\n",temp2);
				}
				break;
			case 106: //weapons
				//?
				break;
			case 107: //size
				//read 2 numbers
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				if (sprite_data[offset]=='-'){offset++;}
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				if (sprite_data[offset]=='-'){offset++;}
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			case 108: //flyingType
				//read one number
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			case 109: //immaterial
				//read two numbers
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				if (sprite_data[offset]=='-'){offset++;}
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				if (sprite_data[offset]=='-'){offset++;}
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			case 110: 
				//read a number and modify the stack with it
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			case 111: //stride
				//read one number
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			case 112: //furniture
				//read one number
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			case 113: //morphEffect
				//read one number
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			case 114: //furnitureSize
				//read one number
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
//				printf("\t\t%d\n",temp);
				break;
			default:
			{
				int a,b,c,d,e,f;
				a = value;
				if (sprite_data[offset] == '=')
				{
					offset++;
//					printf("Mystery: %d\n", atoi((char*)&sprite_data[offset]));
					while (isdigit(sprite_data[offset])) { offset++; }
				}
				else
				{
					while (isspace(sprite_data[offset])) { offset++; }
					b = atoi((char*)&sprite_data[offset]);
					while (isspace(sprite_data[offset])) { offset++; }
					while (isdigit(sprite_data[offset])) { offset++; }
					while (!isdigit(sprite_data[offset])) { offset++; }
					c = atoi((char*)&sprite_data[offset]);
					while (isdigit(sprite_data[offset])) { offset++; }
					while (!isdigit(sprite_data[offset])) { offset++; }
					//b is an unknown value (usually 1, but sometimes 0,2,3,4,5,6,7)
					(*sprite_defs)[spr_num].actions[a].num_frames = c;
					(*sprite_defs)[spr_num].actions[a].frames = new sprite_action_frame[c];
					for (int i = 0; i < c; i++)
					{
						while (isspace(sprite_data[offset])) { offset++; }
					//	printf("\t\t'%.10s' ", &sprite_data[offset]);
						d = atoi((char*)&sprite_data[offset]);
						while (isdigit(sprite_data[offset])) { offset++; }
						while (!isdigit(sprite_data[offset])) { offset++; }
						while (isspace(sprite_data[offset])) { offset++; }
						e = atoi((char*)&sprite_data[offset]);
						while (isdigit(sprite_data[offset])) { offset++; }
						while (!isdigit(sprite_data[offset])) { offset++; }
						while (isspace(sprite_data[offset])) { offset++; }
						f = atoi((char*)&sprite_data[offset]);
						while (isdigit(sprite_data[offset])) { offset++; }
						switch (sprite_data[offset])
						{
							int g;
							case '!':
								//printf("Found a character: %c, ", sprite_data[offset]);
								offset++;
								break;
							//play audio clip?
							case '<':
							case '[':
							case '^':
							case ']':
							case '{':
							case '>':
								//printf("Found a character: %c, ", sprite_data[offset]);
								offset++;
								g = atoi((char*)&sprite_data[offset]);
								while (isdigit(sprite_data[offset])) { offset++; }
								//printf("%d\n", g);
								break;
							default:
								break;
						}
						while (!isspace(sprite_data[offset])) { offset++; }
						(*sprite_defs)[spr_num].actions[a].frames[i].sprite = ani_num;
						(*sprite_defs)[spr_num].actions[a].frames[i].sub_sprite = d;
						(*sprite_defs)[spr_num].actions[a].frames[i].sub_frame = e;
						(*sprite_defs)[spr_num].actions[a].frames[i].length = f;
						(*sprite_defs)[spr_num].actions[a].frames[i].audio = -1;
						//printf("\t\t\t(sprite %d, anim %d, action %d, sub-anim %d, frame %d, d=%d e=%d f=%d\n",
						//	spr_num, ani_num, a, d, e, d,e,f);
					}
				}
			}
				break;
		}
		}
		}
	}
	printf("There were %d sprites total.\n", spr_num);
	delete [] sprite_data;
	delete getfiles;
}

void sprite::set_action(int action)
{
	
}

void sprite::load(int x, int y, int sprnum)
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	//check to see that all the proper sprite_motions are loaded
	cur_action = 1;
	sprite_num = sprnum;
	for (int i = 0; i < 72; i++)
	{
		int j;
		for (j = 0; j < myclient->sprite_data[sprnum].actions[i].num_frames; j++)
		{
			sprite::load_sprite_gfx(myclient->sprite_data[sprnum].actions[i].frames[j].sprite<<16 |
				myclient->sprite_data[sprnum].actions[i].frames[j].sub_sprite);
//			printf("Checking for %d-%d.spr frame %d\n", 
//				myclient->sprite_data[sprnum].actions[i].frames[j].sprite,
//				myclient->sprite_data[sprnum].actions[i].frames[j].sub_sprite,
//				myclient->sprite_data[sprnum].actions[i].frames[j].sub_frame);
		}
	}
	cur_heading = 0;
	//cycle through each action
		//through each frame
			//check to see if the referenced sprite_motion is loaded
	SDL_mutexV(delay_mtx);
}

//arg2 + frames[frame_num].tiles[i].x - mapX
//arg3 + frames[frame_num].tiles[i].y - mapY
//tile
//h

screen_coord sprite::get_screen()
{
	map_coord temp(0, 0);
	return temp.get_screen();//sprite::sprite_gfx[cur_action].get_screen();
}

#define OPTIONA
//#undef OPTIONA
#define OPTIONB
#undef OPTIONB

void sprite::draw(SDL_Surface *display)
{
#ifdef OPTIONA
	//check time
	//advance frame if enough time has passed
	static Uint32 change_time = SDL_GetTicks();
	if (change_time <= SDL_GetTicks())
	{
		cur_frame++;
		change_time = SDL_GetTicks() + 200;
		if (cur_frame >= myclient->sprite_data[sprite_num].actions[cur_action].num_frames)
			cur_frame = 0;
	}
	if (myclient->sprite_data[sprite_num].actions[cur_action].num_frames > 0)
	{
		const sprite_motion &temp = sprite_gfx[
			myclient->sprite_data[sprite_num].actions[cur_action].frames[cur_frame].sprite<<16 |
			myclient->sprite_data[sprite_num].actions[cur_action].frames[cur_frame].sub_sprite];
		//printf("Frame %d\n", myclient->sprite_data[sprite_num].actions[0].frames[0].sub_frame + cur_heading);
		temp.drawat(400, 300, 
			myclient->sprite_data[sprite_num].actions[cur_action].frames[cur_frame].sub_frame, 
			display);
	}
#endif
#ifdef OPTIONB
	//check time
	//advance frame if enough time has passed
	static Uint32 change_time = SDL_GetTicks();
	if (change_time <= SDL_GetTicks())
	{
		cur_frame++;
		change_time = SDL_GetTicks() + 200;
		if (cur_frame >= myclient->sprite_data[sprite_num].actions[cur_action].num_frames)
			cur_frame = 0;
	}
	if (myclient->sprite_data[sprite_num].actions[cur_action].num_frames > 0)
	{
		const sprite_motion &temp = sprite_gfx[
			myclient->sprite_data[sprite_num].actions[cur_action].frames[cur_frame].sprite<<16 |
			myclient->sprite_data[sprite_num].actions[cur_action].frames[cur_frame].sub_sprite];
		//printf("Frame %d\n", myclient->sprite_data[sprite_num].actions[0].frames[0].sub_frame + cur_heading);
		temp.drawat(400, 300, 
			myclient->sprite_data[sprite_num].actions[cur_action].frames[cur_frame].sub_frame, 
			display);
	}
#endif
//	drawat(400, 300, display);
}

void sprite::change_heading(uint8_t heading)
{
	if (heading >= 0)
	{
		if (heading != cur_action)
		{
	/*		expand_motion(heading);
			cur_action = heading;
			motions[cur_action] = new sprite_motion(posx, posy, myclient);
			char temp[25];
			sprintf(temp, "%d-%d.spr", sprite_num, cur_action);
			motions[cur_action]->delay_load(posx, posy, temp);
			motions[cur_action]->move(posx, posy);*/
		}
	}
}

void sprite::print_actions()
{
	printf("Valid sprite actions [%d]:\n\t", sprite_num);
	for (int cur_action = 0; cur_action < 72; cur_action++)
	{
		int num_frames = myclient->sprite_data[sprite_num].actions[cur_action].num_frames;
		if (num_frames > 0)
		{
			printf("%d\t", cur_action);
			printf("\tsprite, subsprite, subframe\n");
			for (int cur_frame = 0; cur_frame < num_frames; cur_frame++)
			{
				printf("\t(%d) %d,%d,%d, %d\n", cur_frame, 
					myclient->sprite_data[sprite_num].actions[cur_action].frames[cur_frame].sprite,
					myclient->sprite_data[sprite_num].actions[cur_action].frames[cur_frame].sub_sprite,
					myclient->sprite_data[sprite_num].actions[cur_action].frames[cur_frame].sub_frame,
					myclient->sprite_data[sprite_num].actions[cur_action].frames[cur_frame].length);
			}
		}
	}
	printf("\n");
}

void sprite::move(int x, int y, int heading)
{
	cur_action = heading;
	if (heading >= 0)
	{
		if (heading != cur_action)
		{
		/*	expand_motion(heading);
			cur_action = heading;
			motions[cur_action] = new sprite_motion(x, y, myclient);
			char temp[25];
			sprintf(temp, "%d-%d.spr", sprite_num, cur_action);
			motions[cur_action]->delay_load(x, y, temp);
			motions[cur_action]->move(x, y);
			posx = x;
			posy = y;*/
		}
	}
	//sprite::sprite_gfx[cur_action].move(x, y);
	posx = x;
	posy = y;
	cur_heading = heading; 
}

void sprite::drawat(int x, int y, SDL_Surface *display)
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	if (myclient->sprite_data[sprite_num].actions[0].num_frames > 0)
	{
		const sprite_motion &temp = sprite_gfx[myclient->sprite_data[sprite_num].actions[0].frames[0].sprite<<16 |
					myclient->sprite_data[sprite_num].actions[0].frames[0].sub_sprite];
		printf("Frame %d\n", myclient->sprite_data[sprite_num].actions[0].frames[0].sub_frame + cur_heading);
		temp.drawat(x, y, myclient->sprite_data[sprite_num].actions[0].frames[0].sub_frame + cur_heading, display);
		if (cur_action != -1)
		{
			//sprite::sprite_gfx[cur_action]->drawat(x, y, display);
		}
	}
	SDL_mutexV(delay_mtx);
}

sprite::~sprite()
{
	SDL_DestroyMutex(delay_mtx);
}