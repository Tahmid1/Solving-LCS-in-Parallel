/* DP_parallel_waveform.cpp
 *   by Brandon Chow and Tahmid Rahman
 *   for CS87 final project
 * Contains code for parallel waveform DP version
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

using namespace std;

#define SIZE 9

pthread_barrier_t   barrier; // barrier synchronization object

struct thread_information{
  int id;
  int* columns;
  string* A;
  string* B;
  int sizeA;
  int sizeB;
  int** table;
  int cols_per_thread;
};

//function prototypes
int lcs_helper1(string A, string B, int a, int b, int num_threads);
static void* lcs_helper2(void * thread_info);
int lcs(string A, string B, int num_threads);
string genRandom(int len);
void test();
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
  for (int tid=0; tid < num_threads; tid++){
    int curr_spot = 0;
    for (int j = tid; j < b+1; j += num_threads){
      thread_infos[tid].columns[curr_spot] = j;
      curr_spot++;
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

  printf("%f, ", range);
  //printTable(table, a+1, b+1);

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
  //cout << "IN THREAD FUNC" << endl;

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

  int num_its;

  if (sizeA > sizeB){
    num_its = sizeA + 1;
  }
  else{
    num_its = sizeB + 1;
  }

  num_its = 2 * num_its;

  for (int rnd = 0; rnd < num_its-1; rnd++){

    for (int i = 0; i < (cols_per_thread + 1); i++){
      //cout << "I: " << i;
      //cout << " " << columns[i] << endl;
      int curr_column = columns[i];
      int index = rnd - curr_column;


      if (index < 0){
        continue;
      }
      else if (index > sizeA){
        continue;
      }
      else{
        if (index == 0){
          table[index][curr_column] = 0;
        }
        else if (curr_column == 0){
          table[index][curr_column] = 0;
        }
        else if (A[index - 1] == B[curr_column - 1]){
          table[index][curr_column] = table[index-1][curr_column-1] + 1;
        }
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
    }
    //cout << "waiting indefinitely" << endl;
    pthread_barrier_wait(&barrier);
    //cout << "nope, escaped" << endl;
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

  int a_len = A.length();
  printf("%d, %d, ", num_threads, a_len); 

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
  printf("%f \n", range);

  return toReturn;
}


/*test
 *
 *test function to verify correctness of DP program
 */
void test(){
  cout << "BEGINNING TEST PHASE: " << endl;
  cout << "======================" << endl;
  int num_threads = 3;

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
