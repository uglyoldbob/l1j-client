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
uint32_t HashSet<Thing>::hash_function(const Thing& t)
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
	current_things.replace(n);
	//advance to the next item, ensuring it does not exist
	//in the unlikely event that all 1<<32 entities have been run through
	do
	{
		uint32_t nid = next_thing.id() + 1;
		nid |= (nid==0);
		next_thing = Thing(nid);
	} while (current_things.lookup(next_thing));
	return n;
}

void ThingAdministrator::destroy(Thing t)
{
	current_things.remove(t);
}
