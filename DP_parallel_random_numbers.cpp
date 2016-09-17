#include <thread>
#include <random>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/time.h>

using namespace std;

#define SIZE 9
#define NUM_THREADS 4

void thread_fcn(int tid);


int main(){
  srand(time(0));
  cout << endl;
  cout << "*** WARNING: NEED TO MAKE THREAD-SAFE RANDOM NUMBERS ***" << endl;
  cout << "*** BUT:... for some reason ... they look thread safe...***"<< endl;
  cout << endl;


  thread* mythreads;
  int num_threads = NUM_THREADS;

  /*
  * Spawn off threads
  */
  mythreads = new thread[num_threads];
  for (int tid=0; tid < num_threads; tid++){
    mythreads[tid] = thread(thread_fcn,tid);
  }


  /*
  * Join Threads
  */
  for (int tid=0; tid < num_threads; tid++){
    mythreads[tid].join();
  }

  delete [] mythreads;
  return 0;
}

void thread_fcn(int tid){

  int random_number;

  string msg;


  for (int i=0; i < 10; i++){
    random_number = rand() % 100;
    msg = "tid "+to_string(tid)+": "+to_string(random_number) +"\n";
    cout << msg;

  }
}
