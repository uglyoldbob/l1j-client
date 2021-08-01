#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <cstdint>
#include <vector>

/*! \brief A hashmap
 This is a hashmap. It uses robin hood hashing on an open addressing scheme.
 It also used fibonacci hashing.
 */

template <class K, class V>
class HashMap
{
private:
	struct item
	{
		K key;
		V value;
	};

public:
	/*! Build a hashmap that starts with the specified number of bits. */
	HashMap(uint8_t size);
private:
	std::vector<item> elements; ///< The list of elements that make up the actual hashmap
	std::vector<uint32_t*> hashes;
	uint32_t num_things; ///< The number of elements currently stored in the hashmap.
	uint32_t resize_threshold;	///< The maximum number of elements that can be added before the map is resized to increase size.
	uint8_t num_bits; ///< The number of bits of size for the hash table. This implies a power of two size.
	uint32_t hash_function(uint32_t); ///< This implements the fibonacci hashing part
};

template <class K, class V>
HashMap<K,V>::HashMap(uint8_t size)
{
	num_things = 0;
}

template <class K, class V>
uint32_t HashMap<K, V>::hash_function(uint32_t h)
{
	//The value is 1<<32 / the golden ratio
	return (h * 2654435769) >> (32-num_bits);
}

#endif