#ifndef _PARTIAL_TABLE_H_
#define _PARTIAL_TABLE_H_

#include "files.h"
#include "table.h"

class partial_table : public table
{
	public:
		virtual void load(const char *real_name, files *place, file_locations location);
		partial_table();
		~partial_table();
};

#endif