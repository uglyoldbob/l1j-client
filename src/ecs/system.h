#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "hashmap.h"
#include "thing.h"

/*! \brief The base class for all systems
*/
class System
{
public:
	System();
private:
	HashSet<Thing> elements; ///< A list of things that have all of the required components for this system.
	int joe;
/*for tracking Things that have all the components a system needs

1. perform filtering on all of the sets on each component, finding the commonality every time the system is run
2. keep a list of things that is updated when the related components are created and destroyed

*/
};

#endif