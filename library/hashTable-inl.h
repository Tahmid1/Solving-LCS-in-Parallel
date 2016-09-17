#include <string>
#include <stdexcept>
#include "arrayQueue.h"
#include <iostream>
/**
 * Given a capacity, constructs a hash table with that initial hash
 * table size.
 */
template <typename K, typename V>
HashTable<K,V>::HashTable(int capacity, double maxLoadFactor) {
  this->capacity = capacity;
  this->size = 0;
  this->MAX_ALPHA = maxLoadFactor;
  this->comparisons = 0;
  this->table = new std::vector<Pair<K,V> >[this->capacity];
}

/**
 * Destructs a hash table.
 */
template <typename K, typename V>
HashTable<K,V>::~HashTable() {
  delete [] this->table;
}

/**
 * Inserts a new key-value pair into the hash table
 * Throws an error if the key already exists
 */
template <typename K, typename V>
void HashTable<K,V>::insert(K key, V value) {
  int pos = hash(key, capacity);            

  // throw error if key already in table
  for (int i = 0; i < table[pos].size(); ++i) {  
    comparisons++;                               
    if (table[pos][i].first == key) {             
      throw std::runtime_error("hashTable::insert key already exists");
    }
  }

  ++size;                                        // Otherwise update the size
  table[pos].push_back(Pair<K,V>(key,value));    // and insert the item into
  if (getLoadFactor() > MAX_ALPHA) {              // the hash table, expanding
    expandCapacity();                            // the table if it exceeds
  }                                              // the max load factor.
}

/**
 * Given a key, returns the value for the key from the hash table.  
 *
 * Throws a runtime_error if the key is not in the hash table.
 */
template <typename K, typename V>
V HashTable<K,V>::find(K key) {
  int pos = hash(key, capacity);
  for(int i = 0; i < table[pos].size(); i++){
    comparisons++;                               // in the hash table
    if(table[pos][i].first == key){
      return table[pos][i].second;
    }
  }
  throw std::runtime_error("find called on key not in HashTable");
}

/**
 * Updates value of key already in HashTable
 * Throws an error if the key doesn't exist
 */
template <typename K, typename V>
void HashTable<K,V>::update(K key, V value) {
  int pos = hash(key, capacity);            

  // throw error if key already in table
  for (int i = 0; i < table[pos].size(); ++i) {  
    comparisons++;                               
    if (table[pos][i].first == key) {             
      table[pos][i] = Pair<K,V>(key,value);
      return;
    }
  }

  // key not found? throw error
  throw std::runtime_error("update called on key not in HashTable");
}


/**
 * Given a key, removes the key-value pair from the hash table
 *
 * Throws a runtime_error if the key is not in the hash table.
 */
template <typename K, typename V>
void HashTable<K,V>::remove(K key) {
  int pos = hash(key, capacity);
  for(int i = 0; i < table[pos].size(); i++){
    comparisons++;                               // in the hash table
    if(table[pos][i].first == key){
      size--;
      table[pos].erase(table[pos].begin()+i);
      return;
    }
  }
  throw std::runtime_error("remove called on key not in HashTable");
}

/**
 * Returns the current number of items in the hash table.
 */
template <typename K, typename V>
int HashTable<K,V>::getSize() {
  return size;
}

/**
 * Returns if the hash table is currently empty.
 */
template <typename K, typename V>
bool HashTable<K,V>::isEmpty() {
  return size == 0;
}

/**
 * Returns if the hash table currently contains the given key.
 */
template <typename K, typename V>
bool HashTable<K,V>::containsKey(K key) {
  int pos = hash(key, capacity);
  for(int i = 0; i < table[pos].size(); i++){
    comparisons++;                               // in the hash table
    if(table[pos][i].first == key){
      return true;
    }
  }
  return false;
}

/**
 * Returns if the hash table currently contains the given value.
 */
template <typename K, typename V>
bool HashTable<K,V>::containsValue(V value) {
  for(int pos = 0; pos < capacity; pos++){
    for(int i = 0; i < table[pos].size(); i++){
    comparisons++;                               // in the hash table
      if(table[pos][i].second == value){
        return true;
      }
    }
  }
  return false;
}


/**
 * Returns a list of all keys currently stored in the hash table.
 *
 * The caller is responsible for deleting this list.
 */
template <typename K, typename V>
std::vector<K> HashTable<K,V>::getKeys() {
  std::vector<K> keys;
  for(int pos = 0; pos < capacity; pos++){
    for(int i = 0; i < table[pos].size(); i++){
      keys.push_back(table[pos][i].first);
    }
  }
  return keys;
}

/**
 * Returns a list of all values currently stored in the hash table.
 *
 * The caller is responsible for deleting this list.
 */
template <typename K, typename V>
std::vector<V> HashTable<K,V>::getValues() {
  std::vector<V> values;
  for(int pos = 0; pos < capacity; pos++){
    for(int i = 0; i < table[pos].size(); i++){
      values.push_back(table[pos][i].second);
    }
  }
  return values;
}

/**
 * Returns an iterator for all key-value pairs currently stored in the 
 * hash table.
 *
 * The caller is responsible for deleting this iterator.
 */
template <typename K, typename V>
Queue<Pair<K,V> >* HashTable<K,V>::getItems() {
  Queue<Pair<K,V> >* it = new ArrayQueue<Pair<K,V> >;

  for(int pos = 0; pos < capacity; pos++){
    for(int i = 0; i < table[pos].size(); i++){
      it->enqueue(table[pos][i]);
    }
  }
  return it;
}

/**
 * Should report information you need for analyzing the performance of the
 * implementation.  This includes information such as the number of 
 * comparisons, size, capacity, load factor, maximum vector length, number
 * of non-zero buckets, average length of a bucket, etc.
 */
template <typename K, typename V>
void HashTable<K,V>::reportStats() {
  std::cout << "Capacity: " << this->capacity << std::endl;
  int maxLength = 0;
  for(int i = 0; i < capacity; i++){
    if(table[i].size() > maxLength){
      maxLength = table[i].size();
    }
  }
  std::cout << "Max List Length: " << maxLength << std::endl;
  //TODO: complete this method
}

//////////////////////////////HELPER METHODS/////////////////////////////////

/**
 * Given a key and a hash table capacity, returns the hash code for that key.
 * This hash code will be the position of the key in the hash table array.
 */
template <typename K, typename V>
int HashTable<K,V>::hash(int key, int capacity) {
  int rv = key % capacity;
  if (rv < 0) {      // Ensuring that our return value is positive, because
    rv += capacity;  // mod in C++ can return a negative value.
  }
  return rv;
}

/**
 * Given a key and a hash table capacity, returns the hash code for that key.
 * This hash code will be the position of the key in the hash table array.
 */
template <typename K, typename V>
int HashTable<K,V>::hash(std::string key, int capacity) {
  int total = 0;
  for (int i = 0; i < key.length(); ++i) {
    total = 31*total + key[i];
  }
  int rv = total % capacity;
  if (rv < 0) {      // Ensuring that our return value is positive, because
    rv += capacity;  // mod in C++ can return a negative value.
  }
  return rv;
}

/**
 * Returns the current load factor of this hash table.
 */
template <typename K, typename V>
double HashTable<K,V>::getLoadFactor() {
  return (float)size/capacity;
}

/**
 * Expands the hash table array, rehashing the contents of the hash table
 * into the new lists.  By doing this, expandCapacity reduces the 
 * load factor of the hash table.
 */
template <typename K, typename V>
void HashTable<K,V>::expandCapacity() {
  std::vector<Pair<K,V> >* newTable = new std::vector<Pair<K,V> >[capacity*2];
  
  for(int pos = 0; pos < capacity; pos++){
    for(int i = 0; i < table[pos].size(); i++){
      int newPos = hash(table[pos][i].first, capacity*2);
      newTable[newPos].push_back(table[pos][i]);
    }
  }
  this->capacity *= 2;
  delete [] this->table;
  this->table = newTable;

}

