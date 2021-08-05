#include "thing.h"

Thing::Thing()
{
	i = 0;
}

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
	current_things(4),
	next_thing((uint32_t)1)
{
}

Thing ThingAdministrator::spawn()
{
	Thing n = next_thing;
	current_things.replace(n, 1);
	uint32_t nid = next_thing.id() + 1;
	nid |= (nid==0);
	next_thing = Thing(nid);
	return n;
}

void ThingAdministrator::destroy(Thing t)
{
}
