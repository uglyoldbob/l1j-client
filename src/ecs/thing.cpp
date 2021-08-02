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
	current_things(10),
	next_thing((uint32_t)0)
{
	int bob = current_things.size();
	current_things.replace(next_thing, 1);
}

Thing ThingAdministrator::spawn()
{
	return Thing(0);
}

void ThingAdministrator::destroy(Thing t)
{
}
