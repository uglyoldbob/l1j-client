#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <cstdint>
#include <vector>

/*! \brief A hashmap
 This is a hashmap. It uses robin hood hashing on an open addressing scheme.
 It also used fibonacci hashing.
 */

#define CALCULATE_DISTANCES 0

template <class K, class V>
class HashMap
{

public:
	/*! Build a hashmap that starts with the specified number of bits. */
	HashMap(uint8_t size, K val);
	uint32_t size(); ///< Returns the number of elements currently stored in the hashmap.
	V* lookup(K key); ///< Lookup the value in the table corresponding to the given key. Return nullptr if not found.
	void replace(K key, V value); ///< Insert or replace an existing value in the hashmap with the value specified.
	void remove(K key); ///< Remove whatever value for the given key exists.
private:
	std::vector<K> keys; ///< The list of keys for the hashmap
	std::vector<V> values; ///< The list of values for the hashmap
	std::vector<uint32_t> hashes; ///< The hash for each element.
	#if CALCULATE_DISTANCES==0
	std::vector<uint32_t> distances; ///< The distance away from ideal for each element.
	#endif
	uint32_t num_things; ///< The number of elements currently stored in the hashmap.
	uint32_t hash_size; ///< The number of elements that could be stored in the hashmap.
	uint32_t resize_threshold;	///< The maximum number of elements that can be added before the map is resized to increase size.
	uint8_t num_bits; ///< The number of bits of size for the hash table. This implies a power of two size.
	uint32_t mask; ///< The mask to use when masking out bits not valid for the size of the table
	uint32_t map_function(uint32_t); ///< This implements the fibonacci hashing part
	uint32_t hash_function(const K& key); ///< The function that creates the first value from a key

	uint32_t lookup_index(K key); ///< Lookup the index for the specified key. A return value of 0xFFFFFFFF indicates it does not exist.
	void drop_key(K key); ///< Drop the given key from the hash table. It is assumed that the key exists in the hash table already.
	void insert(K key, V value); ///< Insert a record into the hash table. This function assumes that the record does not already exist. This is why it is a private function.
	void insert_qualified(uint32_t hash, K key, V value); ///< Inserts an hask, key, value pair into the table. This function assumes the record does not already exist.
	void full_insert(uint32_t hash, uint32_t position, uint32_t distance, K key, V value); ///< The fully specified form for inserting an item into the hash table.
	#if CALCULATE_DISTANCES==1
	uint32_t get_distance(uint32_t hash, uint32_t position); ///< Returns the distance from ideal position of the given hash and the specified position
	#endif
};

template <class K, class V>
HashMap<K,V>::HashMap(uint8_t size, K val)
{
	num_things = 0;
	keys.resize(1<<size, val);
	values.resize(1<<size);
	hashes.resize(1<<size);
	#if CALCULATE_DISTANCES==0
	distances.resize(1<<size);
	#endif
	num_bits = size;
	hash_size = (1<<size);
	mask = (1<<size)-1;
	resize_threshold = (1<<size) * 0.9;
}

template <class K, class V>
uint32_t HashMap<K, V>::map_function(uint32_t h)
{
	//The value is 1<<32 / the golden ratio
	uint32_t calc = ((uint32_t)(h * 2654435769))>> (32-num_bits);
	//a hash of 0 indicates that a sot is unused, so ensure that the hash never returns that value
	//otherwise a filled slot might accidentally be considered as unused.
	calc |= (calc == 0); //this changes a 0 to a 1
	return calc;
}

template <class K, class V>
uint32_t HashMap<K, V>::size()
{
	return num_things;
}

template <class K, class V>
void HashMap<K, V>::insert(K key, V value)
{
	if (++num_things >= resize_threshold)
	{
		//\todo increase size of the hash map
	}
	insert_qualified(hash_function(key), key, value);
}

template <class K, class V>
void HashMap<K, V>::insert_qualified(uint32_t hash, K key, V value)
{
	uint32_t position = map_function(hash); // The position being considered for the key and value
	uint32_t distance = 0; // The distance from perfect of where it is being considered for placement
	bool done = false;
	while (!done)
	{
		// if the current slot is empty, fill it
		if (hashes[position] == 0)
		{
			values[position] = value;
			keys[position] = key;
			hashes[position] = hash;
			distances[position] = distance;
			done = true;
			break;
		}
		#if CALCULATE_DISTANCES == 0
		else if (distances[position] < position)
		#else
		else if (get_distance(hash, position) < position)
		#endif
		{	//if the current position is closer to perfect position, then
			//swap that element with the one we are currently inserting
			//and continue by finding a place for that element
			uint32_t temp_hash = hashes[position];
			hashes[position] = hash;
			hash = temp_hash;

			K temp_key(keys[position]);
			keys[position] = key;
			key = temp_key;

			V temp_val(values[position]);
			values[position] = value;
			value = temp_val;
			
			uint32_t temp_distance = distances[position];
			distances[position] = distance;
			distance = temp_distance;
		}
		position = (position+1) & mask;
		distance++;
	}
}

template <class K, class V>
uint32_t HashMap<K, V>::lookup_index(K key)
{
	uint32_t hash = hash_function(key);
	uint32_t position = map_function(hash);
	uint32_t distance = 0;
	bool done = false;
	while (!done)
	{
		if (	(hashes[position] == 0) ||
			(distance > distances[position]) )
		{	//the element is not present
			position = 0xFFFFFFFF;
			done = true;
			break;
		}
		else if ((hashes[position] == hash) &&
			(keys[position] == key) )
		{
			done = true;
			break;
		}
		position = (position+1) & mask;
		distance++;
	}
	
	return position;
}

template <class K, class V>
V* HashMap<K, V>::lookup(K key)
{
	V* found = nullptr;
	uint32_t index = lookup_index(key);
	if (index != 0xFFFFFFFF)
	{
		found = &values[index];
	}
	
	return found;
}

template <class K, class V>
void HashMap<K, V>::replace(K key, V value)
{
	V* look = lookup(key);
	if (look != nullptr)
	{
		*look = value;
	}
	else
	{
		insert(key, value);
	}
}

template <class K, class V>
void HashMap<K, V>::remove(K key)
{
	V* look = lookup(key);
	if (look != nullptr)
	{
		drop_key(key);
	}
}

template <class K, class V>
void HashMap<K, V>::drop_key(K key)
{
	// /todo implement the backward shift deletion
}

#endif