#include "thing.h"

Thing::Thing(uint32_t s)
{
	i = s;
}

ThingAdministrator::ThingAdministrator() : 
	next_thing((uint32_t)0)
{
}

Thing ThingAdministrator::spawn()
{
	return Thing(0);
}

void ThingAdministrator::destroy(Thing t)
{
}
