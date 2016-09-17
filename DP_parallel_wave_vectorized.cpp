/* DP_parallel_waveform.cpp
 *   by Brandon Chow and Tahmid Rahman
 *   for CS87 final project
 *   Contains code for parallel waveform DP version
 *   of finding the longest common subsequence
 */

/*
#include <thread>
#include <mutex>
*/

#include <pthread.h>

#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/time.h>
#include <vector>

using namespace std;

#define SIZE 9

pthread_barrier_t   barrier; // barrier synchronization object

struct Pair{
  int index;
  int curr_column;
};

struct thread_information{
  int id;
  int* columns;
  string* A;
  string* B;
  int sizeA;
  int sizeB;
  int** table;
  int cols_per_thread;
  vector<Pair>* work_pairs;
};


//function prototypes
int lcs_helper1(string A, string B, int a, int b, int num_threads);
static void* lcs_helper2(void * thread_info);
int lcs(string A, string B, int num_threads);
string genRandom(int len);
void test();
void check_many(int num_threads, int num_runs);
void printTable(int** table, int a_len, int b_len);
int** table;


/* lcs_helper1
 * @ inputs: A - a string to LCS with
 * @ inputs: B - another string to LCS A with
 * @ inputs: a - the length of string A
 * @ inputs: b - the length of string B
 * @ inputs: num_threads - the number of threads to be used
 *
 * this function is responsible for initializing all the state associated 
 * with each thread
 * 
 * furthermore, this function spawns num_threads many worker threads to 
 * start solving LCS in parallel
 *
 * it creates and joins the threads using the pthread library
 */
int lcs_helper1(string A, string B, int a, int b, int num_threads){

  //int** table;
  table = new int*[a+1];
  for (int i=0; i < a+1; i++){
    table[i] = new int[b+1];
  }

  // Set up
  thread_information* thread_infos = new thread_information[num_threads]();
  int cols_per_thread = ((b+1)/num_threads);

  for (int tid = 0; tid < num_threads; tid++){
    //cout << "TID ON FIRST SETUP: " << tid << endl;
    thread_infos[tid].id = tid;
    thread_infos[tid].A = &A;
    thread_infos[tid].B = &B;
    thread_infos[tid].sizeA = a;
    thread_infos[tid].sizeB = b;
    thread_infos[tid].table = table;
    thread_infos[tid].cols_per_thread = cols_per_thread;

    //initialize all column info to be a very large number
    thread_infos[tid].columns = new int[cols_per_thread + 1];
    for (int i = 0; i < cols_per_thread + 1; i++){
      thread_infos[tid].columns[i] = (a*b);
    }
  }

  //now fill it with useful information
  //namely, the column numbers associated with each thread
  for (int tid=0; tid < num_threads; tid++){
    int curr_spot = 0;
    for (int j = tid; j < b+1; j += num_threads){
      thread_infos[tid].columns[curr_spot] = j;
      curr_spot++;
    }
  }

  //the following calculation with num_its figures out the number of iterations
  //needed to run to get the final answer
  int num_its;

  if (a > b){
    num_its = a + 1;
  }
  else{
    num_its = b + 1;
  }
  num_its = 2 * num_its;

  //looping through tids now, and setting up vector array with each tid
  //vector array is going to be house vectors of pairs

  //the pairs are (index, column) of each table entry

  //a vector stored in the array indexed at round will store pairs for that 
  //tid for that round 

  //looping through tids
  for (int tid = 0; tid < num_threads; tid++){
    //creates array of vectors with one bucket per round
    thread_infos[tid].work_pairs = new vector< Pair >[num_its-1];

    //goes through rounds
    for (int rnd = 0; rnd < num_its-1; rnd++){
      //goes through columns
      for (int i = 0; i < (cols_per_thread + 1); i++){

        //calculates some math to figure out the current index for that column
        //to focus on based on the current round
        int curr_column = thread_infos[tid].columns[i];
        int index = rnd - curr_column;

        //if the index is valid, then the (index, column) pair are pushed
        //onto the vector
        if (index < 0){
          continue;
        }
        else if (index > a){
          continue;
        }

        //this is an extraneous check, and can be removed if need be
        else if (curr_column > b){
          continue;
        }
        else{
          Pair curr_work_pair;
          curr_work_pair.index = index;
          curr_work_pair.curr_column = curr_column;
          thread_infos[tid].work_pairs[rnd].push_back(curr_work_pair);
        }
      }
    }
  }

  //set up array of threads
  pthread_t*  mythreads = new pthread_t[num_threads];

  struct timeval start, end;

  gettimeofday(&start, NULL);

  //spawn off the threads
  for (int tid=0; tid < num_threads; tid++){
    pthread_create(&(mythreads[tid]), NULL, lcs_helper2, (void *) &thread_infos[tid]);
  }

  //join all threads back together
  for (int tid=0; tid < num_threads; tid++){
    pthread_join(mythreads[tid], NULL);
  }


  gettimeofday(&end, NULL);

  //calculate difference in times
  float range = (((end.tv_sec * 1000000) + end.tv_usec) -
      ((start.tv_sec * 1000000) + start.tv_usec))/1000000.0;

  //prints the time it took to run after all of the state has been initialized
  printf("Time after state init: %f \n", range);

  int toReturn = table[a][b];

  //cleaning up table
  for (int i=0; i < a+1; i++){
    delete [] table[i];
  }
  delete [] table;

  for (int tid = 0; tid < num_threads; tid++){
    delete thread_infos[tid].columns;
  }

  delete thread_infos;
  delete mythreads;

  //returning the answer
  return toReturn;



}

/* lcs_helper2
 * @inputs: thread_info - a pointer to a thread_information struct
 *
 * this function defines the behavior for a single thread, using the 
 * information for a struct
 *
 * it runs through the LCS algorithm to fill out round by round the cells 
 * that a thread is responsible for
 *
 * there is a barrier between rounds so that each wave is done in parallel, 
 * but further computation doesn't start before it is safe to do so
 */
static void* lcs_helper2(void * thread_info){

  struct thread_information* thread_information
    = (struct thread_information * ) thread_info;

  int id = thread_information -> id;
  int* columns = thread_information -> columns;

  string* ptrA = thread_information -> A;
  string* ptrB = thread_information -> B;

  string A = *ptrA;
  string B = *ptrB;

  int sizeA = thread_information -> sizeA;
  int sizeB = thread_information -> sizeB;

  int** table = thread_information -> table;
  int cols_per_thread = thread_information -> cols_per_thread;

  vector<Pair>* work_pairs = thread_information -> work_pairs;

  int num_its;

  if (sizeA > sizeB){
    num_its = sizeA + 1;
  }
  else{
    num_its = sizeB + 1;
  }

  num_its = 2 * num_its;

  //looping through the number of rounds
  for (int rnd = 0; rnd < num_its-1; rnd++){

    //getting the vector associated with this thread and current round
    vector<Pair> curr_vec = work_pairs[rnd];

    for (int i = 0; i < curr_vec.size(); i++){
      //looping through all pairs stored in the vector
      Pair curr_pair = curr_vec[i];

      //get the index and column values from the pair
      int curr_column = curr_pair.curr_column;
      int index = curr_pair.index;

      //the table denotes the LCS of A[:index] and B[:curr_column]
      //if index is 0, then we know we are comparing to an empty string
      if (index == 0){
        table[index][curr_column] = 0;
      }

      //similarly, if curr_column is 0, we are comparing to an empty string
      else if (curr_column == 0){
        table[index][curr_column] = 0;
      }

      //otherwise, compare the last two letters

      //if they match up
      else if (A[index - 1] == B[curr_column - 1]){
        table[index][curr_column] = table[index-1][curr_column-1] + 1;
      }
      //if they don't match up
      else{
        int max_sublen1 = table[index-1][curr_column];
        int max_sublen2 = table[index][curr_column-1];

        //finding the max of the two sublens
        if (max_sublen1 > max_sublen2){
          table[index][curr_column] = max_sublen1;
        }
        else{
          table[index][curr_column] = max_sublen2;
        }
      }
    }
    pthread_barrier_wait(&barrier);
  }
  void* garbage;
  return garbage;
}


/* lcs (string A, string B)
 * @ inputs: A - one of the strings for LCS
 * @ inputs: B - another of the strings for LCS
 * @ inputs: num_threads - the number of threads for running LCS
 *
 * finds the longest common subsequence of strings A and B
 * also times the run and outputs the time
 */
int lcs(string A, string B, int num_threads){

  int toReturn;

  struct timeval start, end;

  gettimeofday(&start, NULL);

  toReturn = lcs_helper1(A, B, A.length(), B.length(), num_threads);

  gettimeofday(&end, NULL);

  //calculate difference in times
  float range = (((end.tv_sec * 1000000) + end.tv_usec) -
      ((start.tv_sec * 1000000) + start.tv_usec))/1000000.0;

  //printf("Time for %d iterations: %f seconds.\n", sims, range);
  //printf("%d, %lu, %f \n", num_threads,A.length(),range);
  //printf("Time for %d iterations: %f seconds.\n", sims, range);
  printf("Time: %f \n", range);

  return toReturn;
}


/*test
 *
 *test function to verify correctness of DP program
 */
void test(){
  cout << "BEGINNING TEST PHASE: " << endl;
  cout << "======================" << endl;
  int num_threads = 1;

  string A = "AEIOU";
  string B = "AOU";

  if(pthread_barrier_init(&barrier, NULL, num_threads)){
    perror("Exiting because of error while initializing barrier.\n");
    exit(1);
  }


  int len = lcs(A, B, num_threads);

  cout << "length found is: " << len << endl;
  cout << "length should be 3" << endl << endl;

  A = "AEIOU";
  B = ":LKSDG";

  if(pthread_barrier_init(&barrier, NULL, num_threads)){
    perror("Exiting because of error while initializing barrier.\n");
    exit(1);
  }

  len = lcs(A, B, num_threads);

  cout << "length found is: " << len << endl;
  cout << "length should be 0" << endl << endl;

  A = "";
  B = ":LKSDG";

  if(pthread_barrier_init(&barrier, NULL, num_threads)){
    perror("Exiting because of error while initializing barrier.\n");
    exit(1);
  }

  len = lcs(A, B, num_threads);

  cout << "length found is: " << len << endl;
  cout << "length should be 0" << endl << endl;

  A = "";
  B = "";

  if(pthread_barrier_init(&barrier, NULL, num_threads)){
    perror("Exiting because of error while initializing barrier.\n");
    exit(1);
  }

  len = lcs(A, B, num_threads);

  cout << "length found is: " << len << endl;
  cout << "length should be 0" << endl << endl;

  A = "AEGJSDIL";
  B = "AEGJSDIL";

  if(pthread_barrier_init(&barrier, NULL, num_threads)){
    perror("Exiting because of error while initializing barrier.\n");
    exit(1);
  }

  len = lcs(A, B, num_threads);

  cout << "length found is: " << len << endl;
  cout << "length should be 8" << endl << endl;


  A = "KWIGLI";
  B = "KIGWLA";

  if(pthread_barrier_init(&barrier, NULL, num_threads)){
    perror("Exiting because of error while initializing barrier.\n");
    exit(1);
  }

  len = lcs(A, B, num_threads);

  cout << "length found is: " << len << endl;
  cout << "length should be 4" << endl << endl; 

}

/* genRandom
 * @ inputs: len - the length of the random string to generate
 *
 * craetes a random string of length len
 */
string genRandom(int len){

  static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  string toReturn = string(len, '0');

  for (int i = 0; i < len; ++i) {
    toReturn[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  toReturn[len] = 0;

  return toReturn;

}

/* printTable
 * @ inputs: table - a table filled out via LCS(a, b)
 * @ inputs: a_len - length of string a
 * @ inpust: b_len - length of string b
 *
 *prints out table, useful for debugging
 */
void printTable(int** table, int a_len, int b_len){
  cout << endl << endl;
  for (int i = 0; i < a_len; i++){
    for (int j = 0; j < b_len; j++){
      cout << " " << table[i][j];
    }
    cout << endl;
  }
  cout << endl << endl;
}

/* check_many
 * @ inputs: num_threads - maximum number of threads
 * @ inputs: num_runs - maximum number of runs
 * @ inputs: A - one of the strings for LCS
 * @ inputs: B - another of the strings for LCS
 * 
 * This function iterates num_runs many trials of LCS for each 
 * thread value up to num_threads
 *
 * It verifies that the same answer is outputted
 */
void check_many(int num_threads, int num_runs, string A, string B){
  for (int n_t = 1; n_t <= num_threads; n_t++){
    cout << "NUM THREADS IS: " << n_t << endl;  
    //initializing barrier
    if(pthread_barrier_init(&barrier, NULL, num_threads)){
      perror("Exiting because of error while initializing barrier.\n");
      exit(1);
    }		
    for (int i = 0; i < num_runs; i++){
      int answer = lcs(A, B, num_threads);
      cout << "LCS determined: " << answer << " as common length" << endl;
    }
    cout << endl << "==========================================" << endl;
  }
}

//main function
int main(int argc, char** argv){
  bool in_testing_mode;
  int answer;
  int size;
  int num_threads;
  int num_runs;

  if (argc > 1){
    if (atoi(argv[1]) == 1){
      if (argc != 4){
        cout << "\nUsage (testing mode): ./dp_mem (in_testing_mode = 1)"; 
        cout << "(size) (num_threads) \n\n";
        exit(0);
      }
      in_testing_mode = true;
      num_runs = 5;
      size = atoi(argv[2]);
      num_threads = atoi(argv[3]);
    }
    else if (atoi(argv[1]) == 0){
      if (argc != 4){
        cout << "\nUsage (experimentation mode): ./dp_mem"; 
        cout << " (in_testing_mode = 0) (size) (num_threads)\n\n";
        exit(0);	
      }
      size = atoi(argv[2]);
      num_threads = atoi(argv[3]);	
    }
    else {
      cout << "\nFirst argument (in_testing_mode) must be 0 (use testing mode)"; 
      cout << "or 1 (use experiment mode)\n\n";
      exit(0);
    }

    srand(time(NULL));

    string A;
    string B;

    A = genRandom(size);
    B = genRandom(size);


    if (in_testing_mode){
      check_many(num_threads, num_runs, A, B);
    }
    else{
      if(pthread_barrier_init(&barrier, NULL, num_threads)){
        perror("Exiting because of error while initializing barrier.\n");
        exit(1);
      }

      answer = lcs(A, B, num_threads);
    }
  }
  else{
    cout << "\nUse the following arguments: (mode) (size) (num_threads) \n\n";
  }
}
