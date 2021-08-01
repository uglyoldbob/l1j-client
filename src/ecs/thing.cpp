#include "thing.h"

Thing::Thing(uint32_t s)
{
	i = s;
}

ThingAdministrator::ThingAdministrator() : 
	current_things(10),
	next_thing((uint32_t)0)
{
	int bob = current_things.size();
}

Thing ThingAdministrator::spawn()
{
	return Thing(0);
}

void ThingAdministrator::destroy(Thing t)
{
}
