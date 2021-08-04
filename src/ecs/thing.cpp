#include "thing.h"

Thing::Thing(uint32_t s)
{
	i = s;
}

template <>
uint32_t HashMap<Thing, unsigned char>::hash_function(const Thing& t)
{
	return t.id();
}

ThingAdministrator::ThingAdministrator() : 
	current_things(4, Thing(0)),
	next_thing((uint32_t)1)
{
}

Thing ThingAdministrator::spawn()
{
	Thing n = next_thing;
	current_things.replace(n, 1);
	next_thing = Thing(next_thing.id()+1);
	return n;
}

void ThingAdministrator::destroy(Thing t)
{
}
