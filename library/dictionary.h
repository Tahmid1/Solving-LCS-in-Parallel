#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <vector>
#include "queue.h"
#include "pair.h"

/**
 * Dictionary defines a templated pure-virtual interface for a dictionary.
 * K is the key type, V the value type
 */
template <typename K, typename V>
class Dictionary {
 public:
  virtual ~Dictionary() {}

  /* insert - inserts the key/value pair into the dictionary.
   * @param key - key for indexing the new element
   * @param value - value associated with the given key
   * @error runtime error if key already exists
   */
  virtual void insert(K key, V value) = 0;

  /* find - returns the value associated with the given key.
   * @param key - index key for element to find
   * @error runtime error if key is not found in the dictionary
   * @return V: value associated with the given key
   */
  virtual V    find(K key) = 0;           

  /* update - finds the element indexed by the given key and updates
   *          its value to the provided value parameter
   * @param key - key for finding the existing element
   * @param value - the new value to store for the given key
   * @error runtime error if the key is not in the dictionary
   */
  virtual void update(K key, V value) = 0;

  /* remove - deletes the element with given key from the dictionary
   * @param key - index key to search for and remove
   * @error runtime error if the key is not found
   */
  virtual void remove(K key) = 0;          

  /* getSize - returns the size of the dictionary.
   * @return int: the number of Key/Value pairs in the dictionary.
   */
  virtual int  getSize() = 0;          

  /* isEmpty - returns true if the dictionary is empty
   * @return bool: true if there are no key/value pairs in the dictionary.
   */
  virtual bool isEmpty() = 0;               

  /* containsKey - returns true if the dictionary contains the
   *               given key
   * @param key - index key to search for
   * @return bool: true if the given key exists in the dictionary.
   */
  virtual bool containsKey(K key) = 0;      

  /* containsValue - returns true if the dictionary contains
   *                 a key/value pair with the given value
   * @param value - value to search for
   * @return bool: true if there is a pair with the given value
   */
  virtual bool containsValue(V value) = 0;

  /* getKeys - returns a list of all keys in the dictionary.
   * @return vector: vector of all keys in the dictionary.
   */
  virtual std::vector<K> getKeys() = 0;    

  /* getValues - returns a list of all values in the dictionary.
   * @return vector: vector of all values
   */
  virtual std::vector<V> getValues() = 0;   

  /* getItems - returns an iterator of all key/value pairs in the
   *            dictionary.
   * @return Queue: iterator of all K/V pairs.
   */
  virtual Queue<Pair<K,V> >* getItems() = 0;

  /* reportStats - for performance reporting.
   */
  virtual void reportStats() = 0; 
};

#endif
