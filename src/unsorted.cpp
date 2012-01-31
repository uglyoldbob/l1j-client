#include <ctype.h>
#include <stdio.h>

#include "client.h"
#include "globals.h"

void client::init_codepage(unsigned int arg)
{
//	printf("STUB init_codepage\n");
//	memset(lead_table, 0, 0x100);
//	for (int i = 0; i < 0x100; i++)
//	{
//		lower_table[i] = to_lower[i];
//	}
}

void client::init_math_tables()
{
//	printf("STUB InitMathTables\n");	
}

int client::getNumber(char **buf)
{
	int is_neg = 0;
	while ((*buf[0] != 0) && (isdigit(*buf[0]) == 0))
	{
		if (*buf[0] == '-')
		{
			is_neg = 1;
		}
		else if (*buf[0] != ' ')
		{
			is_neg = 0;
		}
		*buf = &buf[0][1];
	}
	int ret_val = 0;
	
	while ((*buf[0] != 0) && (isdigit(*buf[0]) != 0))
	{
		ret_val = ret_val*10 + *buf[0] - '0';
		*buf = &buf[0][1];
	}
	if (is_neg != 0)
	{
		ret_val = -1 * ret_val;
	}
	return ret_val;
}

void client::LoadDurationTable(const char *file)
{
	char *table;
	int length;
	table = textpack->load_file(file, &length, 1);
	char *buffer = table;
	if (table != 0)
	{
		while (buffer[0] != 0)
		{
			getNumber(&buffer);
			int arg1, arg2;
			arg1 = getNumber(&buffer);
			arg2 = getNumber(&buffer);
			if (arg1 != 0)
			{
//				printf("STUB UpdateEffectInfo %d, %d\n", arg1, arg2);
//				UpdateEffectInfo(arg1, arg2);
			}
		}
		delete [] table;
	}
}
