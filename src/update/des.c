unsigned char iperm[0x800];
	//iperm is 0x800 bytes long
	//is a multi-dimensional array?
	
void kinit()
{
	
}

void perminit(unsigned char *perm, unsigned char *p)
{
	unsigned char *pointer1;
	
	unsigned int c1;
	for (c1 = 0; c1 <= 0xF; c1++)
	{
		pointer1 = &perm[c1 * 128];
		int c2;
		for (c2 = 0; c2 <= 0xF; c2++)
		{
			int c3;
			int offset = 0;
			for (c3 = 8; c3 > 0; c3--)
			{
				pointer1[offset] = 0;
				offset++;
			}
			pointer1 = &(pointer1[8]);
		}
	}
	
	
	//redo everything after this comment within this function
	
	unsigned short *spointer1;
	unsigned char *spointer2;
	
	//TODO: change these addresses!
	spointer1 = (unsigned short *)0x2c358;
	spointer2 = (unsigned char *)0x2c338;
		
	for (c1 = 0; c1 <= 0xF; c1++)
	{
		unsigned int c2;
		//r0 = c1 * 128
		for (c2 = 0; c2 <= 0x10; c2++)
		{
			unsigned int offset = 0;
			unsigned int c3;
			//r6 = r0 + r3 = &perm[c1 * 128]
			for (c3 = 0x40; c3 > 0; c3--)
			{
				char short_loop = p[offset] - 1;
				unsigned char *short_loop_p;
				if ((short_loop / 4) == c1)
				{
					unsigned int offset2 = (short_loop * 4) & 0xC;
					short_loop = c2 & spointer1[offset2 / sizeof(short)];
					if (short_loop != 0)
					{
						unsigned int r9 = offset / 8;
						short_loop_p = &spointer2[offset<<2 & 0x1C];
						unsigned char r0 = perm[c1 * 128 + r9];
						r9[c1 * 128 + perm] = short_loop_p[3] | r0;
					}
				}
				offset++;
			}
		}
	}
	return;
}

void DesKeyInit(char *password)
{
	unsigned char *temp1;	//r1[0x40]
	unsigned short temp2 = 0;	//r1[0x44]
	//some stuff before this loop?
	int c;
	
	int test;
	int test2 = 0;
	c = 0;
	while (test2 <= 0x27)
	{
		test = test2 / 8;
		test = test * 8;
		test = test2 - test;
		test2++;
		temp1[test] = temp1[test] ^ password[c];
		
		c++;
		if (password[c] == 0)
			break;
	}
	perminit(iperm, 0);
	//TODO: change these addresses!
	perminit((unsigned char*)0x580f0, (unsigned char*)0x2c060);
	kinit();
	//sinit();
	//p32init();
}

void DesMem(void *buf, int size, int encrypt)
{
	
}

void DesReadBlock(void *buf, int size)
{
	DesMem(buf, size & 0xFFFFFFF0, 0);
}

void DesWriteBlock(void *buf, int size)
{
	DesMem(buf, size & 0xFFFFFFF0, 1);
}


int main()
{
	int a = 5;
	a++;
	DesKeyInit("~!@#%^$<");
	return 0;
}
