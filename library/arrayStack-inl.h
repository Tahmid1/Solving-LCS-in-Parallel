// Implement an ArrayStack here.

#include <iostream>


/*Constructor -
 *
 *
 * ArrayStack utilizes circularArrayList
 */
template <typename T>
ArrayStack<T>::ArrayStack(){

}

/*Destructor -
 *
 */
template <typename T>
ArrayStack<T>::~ArrayStack(){
  //nothing happens here...?
}


/*push - pushes new item onto stack
 *
 */
template <typename T>
void ArrayStack<T>::push(T item){
  items.insertAtTail(item);
}


/* pop - removes and returns first item on stack
 *
 */
template <typename T>
T ArrayStack<T>::pop(){
  return items.removeTail();
}


/*getSize -returns the number of items on stack
 *
 */
template <typename T>
int ArrayStack<T>::getSize(){
  return items.getSize();
}


/* isEmpty - returns true if empty, false if not
 *
 */
template <typename T>
bool ArrayStack<T>::isEmpty(){
  return items.isEmpty();
}


/* getTop - returns (*but does not removes!*) top item
 *
 */
template <typename T>
T ArrayStack<T>::getTop(){
  return items.peekTail();
}

