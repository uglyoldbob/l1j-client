#ifndef __UNSORTED_H_
#define __UNSORTED_H_

void init_codepage(unsigned long arg);
void init_math_tables();
int getNumber(char **buf);
void LoadDurationTable(const char *file);
unsigned short reverse(unsigned short me);
unsigned long reverse(unsigned long me);

#endif