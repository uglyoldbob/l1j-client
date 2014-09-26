#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *spr;
	spr = fopen("sprite_table.txt", "r");
	unsigned char *sprite_data;
	int sprite_dlength;
	fseek(spr, 0, SEEK_END);
	sprite_dlength = ftell(spr);
	rewind(spr);
	sprite_data = (unsigned char*) malloc (sprite_dlength);
	fread(sprite_data, 1, sprite_dlength, spr);
//	sprite_data = (unsigned char*)getfiles->load_file("sprite_table.txt", &sprite_dlength, FILE_REGULAR2, 0);
	for (int i = 0; i < sprite_dlength;i++)
	{
		if (sprite_data[i] == 0xd)
			sprite_data[i] = '\n';
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
		printf("The number is %d (sprite number 0)\n", atoi((char*)&sprite_data[offset]));
		while (isdigit(sprite_data[offset])) { offset++; }
		if (sprite_data[offset] == '=')
		{
			offset += 1;
			int alias = atoi((char*)&sprite_data[offset]);
			//alias replaces? the number used for animations
			printf("Alias of %d found\n", alias);
			while (isdigit(sprite_data[offset])) { offset++; }
			while (!isdigit(sprite_data[offset])) { offset++; }
		}
		while (offset < sprite_dlength)
		{
		while (isspace(sprite_data[offset])) { offset++; }
		if (sprite_data[offset] == 0) 
		{
			offset++;
			static int spr_num = 0;
			spr_num++;
			//printf("'%.15s' ", &sprite_data[offset]);
			printf("The number is %d (sprite number %d)\n", atoi((char*)&sprite_data[offset]), spr_num);
			while (isdigit(sprite_data[offset])) { offset++; }
			if (sprite_data[offset] == '=')
			{
				offset += 1;
				int alias = atoi((char*)&sprite_data[offset]);
				printf("\tAlias of %d found\n", alias);
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
		printf("\t'%.10s' ", &sprite_data[offset]);
		int value = atoi((char*)&sprite_data[offset]);
		while (isdigit(sprite_data[offset])) { offset++; }
		int temp;
		printf("\tmode=%d\n", value);	
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
				printf("\t\t%d\n",temp);
				break;
			case 102: //objType
				//read one value
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				break;
			case 103: //altAttack
				//read one value
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				break;
			case 104: //attr
				//read one value
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				break;
			case 105: //clothing
				//read a value
				//read 0 or that value of numbers, whichever is higher
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				for (int i = 0; i < temp; i++)
				{
					while (isspace(sprite_data[offset])) { offset++; }
					int temp2 =atoi((char*)&sprite_data[offset]);
					while (isdigit(sprite_data[offset])) { offset++; }
					printf("\t\t\t%d\n",temp2);
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
				printf("\t\t%d\n",temp);
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				if (sprite_data[offset]=='-'){offset++;}
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				break;
			case 108: //flyingType
				//read one number
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				break;
			case 109: //immaterial
				//read two numbers
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				if (sprite_data[offset]=='-'){offset++;}
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				if (sprite_data[offset]=='-'){offset++;}
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				break;
			case 110: 
				//read a number and modify the stack with it
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				break;
			case 111: //stride
				//read one number
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				break;
			case 112: //furniture
				//read one number
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				break;
			case 113: //morphEffect
				//read one number
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				break;
			case 114: //furnitureSize
				//read one number
				while (isspace(sprite_data[offset])) { offset++; }
				temp = atoi((char*)&sprite_data[offset]);
				while (isdigit(sprite_data[offset])) { offset++; }
				printf("\t\t%d\n",temp);
				break;
			default:
			{
				int a,b,c,d,e,f;
				a = value;
				if (sprite_data[offset] == '=')
				{
					offset++;
					atoi((char*)&sprite_data[offset]);
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
					printf("\t\ta=%d b=%d c=%d\n", a,b,c);
					for (int i = 0; i < c; i++)
					{
						while (isspace(sprite_data[offset])) { offset++; }
						printf("\t\t'%.10s' ", &sprite_data[offset]);
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
								printf("Found a character: %c, ", sprite_data[offset]);
								offset++;
								break;
							//play audio clip?
							case '<':
							case '[':
							case '^':
							case ']':
							case '{':
							case '>':
								printf("Found a character: %c, ", sprite_data[offset]);
								offset++;
								g = atoi((char*)&sprite_data[offset]);
								while (isdigit(sprite_data[offset])) { offset++; }
								printf("%d\n", g);
								break;
							default:
								break;
						}
						while (!isspace(sprite_data[offset])) { offset++; }
						printf("\t\t\td=%d e=%d f=%d\n",d,e,f);
					}
				}
			}
				break;
		}
		}
		}
	}

	free(sprite_data);

	return 0;
}

