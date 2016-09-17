#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <string>
#include <vector>
#include "dictionary.h"
#include "pair.h"
#include "queue.h"
/**
 * Defines a HashTable, an implementation of the Dictionary interface that
 * stores data using a hash table.  Collisions in this hash table are resolved
 * with separate chaining.
 */
template <typename K, typename V>
class HashTable : public Dictionary<K,V> {
 private:
  // The hash table data, with each entry in the array storing a
  // vector of key/value pairs
  std::vector<Pair<K,V> >* table;

  // The number of items currently stored in the hash table.
  int size;                      

  // The current size of the table array.
  int capacity;                     

  // Should expand hash table if we exceed this load factor.
  double MAX_ALPHA;                 

  // To help report statistics
  int comparisons;          
    
 public:

  /* constructs hash table with default capacity of 53,
   *  maxAlpha of 0.8
   */
  HashTable(int capacity = 53, double maxLoadFactor = 0.8);

  ~HashTable();

  /* insert - inserts the key/value pair into the dictionary.
   * @param key - key for indexing the new element
   * @param value - value associated with the given key
   * @error runtime error if key already exists
   */
  void insert(K key, V value); 

  /* find - returns the value associated with the given key.
   * @param key - index key for element to find
   * @error runtime error if key is not found in the dictionary
   * @return V: value associated with the given key
   */
  V find(K key);           

  /* update - finds the element indexed by the given key and updates
   *          its value to the provided value parameter
   * @param key - key for finding the existing element
   * @param value - the new value to store for the given key
   * @error runtime error if the key is not in the dictionary
   */
  void update(K key, V value);

  /* remove - deletes the element with given key from the dictionary
   * @param key - index key to search for and remove
   * @error runtime error if the key is not found
   */
  void remove(K key);         

  /* getSize - returns the size of the dictionary.
   * @return int: the number of Key/Value pairs in the dictionary.
   */
  int      getSize();           

  /* isEmpty - returns true if the dictionary is empty
   * @return bool: true if there are no key/value pairs in the dictionary.
   */
  bool     isEmpty();           

  /* containsKey - returns true if the dictionary contains the
   *               given key
   * @param key - index key to search for
   * @return bool: true if the given key exists in the dictionary.
   */
  bool     containsKey(K key);  

  /* containsValue - returns true if the dictionary contains
   *                 a key/value pair with the given value
   * @param value - value to search for
   * @return bool: true if there is a pair with the given value
   */
  bool     containsValue(V value);  

  /* getKeys - returns a list of all keys in the dictionary.
   * @return vector: vector of all keys in the dictionary.
   */
  std::vector<K> getKeys();         

  /* getValues - returns a list of all values in the dictionary.
   * @return vector: vector of all values
   */
  std::vector<V> getValues();       

  /* getItems - returns an iterator of all key/value pairs in the
   *            dictionary.
   * @return Queue: iterator of all K/V pairs.
   */
  Queue<Pair<K,V> >* getItems();    
                                      
  /* reportStats - for performance reporting.
   */
  void reportStats();               
                                      

 private:
  // returns a hash code for the given int.
  int hash(int key, int capacity);  

  // returns a hash code for the given string.
  int hash(std::string key, int capacity);

  // returns the current load factor of the hash table.
  double getLoadFactor();               

  // expands capacity of the hash table's array.
  void expandCapacity();                 
};

#include "hashTable-inl.h"

#endif
