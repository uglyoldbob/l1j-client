#ifndef __THING_H__
#define __THING_H__

#include "hashmap.h"
#include <cstdint>



/*! \brief The entity of an entity component system design

The thing class is for identifying entities in an entity component system design
*/
class Thing
{
public:
	Thing(uint32_t s); ///< Create a thing with the given id value.
	inline uint32_t id() const { return i; }
private:
	uint32_t i;
};

inline bool operator==(const Thing& lhs, const Thing& rhs)
{
	return lhs.id() == rhs.id();
}

/*! \brief entity/Thing administration class. Responsible for creating and destroying Things */
class ThingAdministrator
{
public:
	ThingAdministrator();
	Thing spawn(); ///< Create a new thing and return it
	void destroy(Thing t); ///< destroy the given thing
private:
	Thing next_thing;
	HashMap<Thing, uint8_t> current_things;
};

#endif
