#ifndef _PARTIAL_TABLE_H_
#define _PARTIAL_TABLE_H_

#include "table.h"

class partial_table : public table
{
	public:
		virtual void load(const char *real_name, pack* from);
};

#endif