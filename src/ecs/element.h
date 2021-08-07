#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include "hashmap.h"
#include "system.h"

/*! \brief Base class for the Component part of an ECS system.
 This class may be completely unnecessary and might go away.
 */
class Element
{
public:
	Element();
private:
	HashSet<System> affected; ///< A list of systems that would be affected by element creation or deletion.
};

#endif