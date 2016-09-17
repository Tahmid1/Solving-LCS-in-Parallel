/* DP_parallel_memoization.cpp
 *   by Brandon Chow and Tahmid Rahman
 *   for CS87 final project
 *   Contains code for Randomized Subproblems approach
 *   of finding the longest common subsequence
 */

#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/time.h>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <cmath>
#include "library/stack.h"
#include "library/arrayStack.h"

using namespace std;


// timing / experiment tools
struct timeval start;
bool display_stats;
long double mean_encountered_already_solved, mean_iterations;
long double var_encountered_already_solved, var_iterations;

// Stores representation of problem
struct problem {
  int x;
  int y;
};

// Default parameters
int num_runs = 1;
int num_threads = 1;
int size = 9;


// Globally shared table and strings
int** table;
string A;
string B;
int a;
int b;


// Stores thread information
struct thread_information{
  int tid;
  int num_threads;
  int start_x;
  int start_y;
  int answer;

  int times_already_solved;
  long num_iters;
  int* random_stream;
  int random_stream_position;
  int random_stream_size;

  struct timeval end_time;
};


void test();
void check_many(int num_runs);
string genRandomString(int len);
int** initTable(int a_len, int b_len);
void printTable(int** table, int a_len, int b_len, int curr_x, int curr_y);
int lcs(int thread_assignment_method);
int lcs_helper1(int num_threads,int thread_assignment_method);
static void*  lcs_helper2(void* thread_data);
void get_dependent_lcs_subproblems(problem* prob,int* num_subprobs, problem* subprobs);
void print_dependent_subproblems(problem* prob, problem* subprobs,int num_subprobs);
float computeTime(struct timeval* end);
long double sample_mean(int* data);
long double sample_var(int* data);


void assign_threads_method1(thread_information* thread_infos, int num_threads);
void assign_threads_method2(thread_information* thread_infos, int num_threads);
void assign_threads_method3(thread_information* thread_infos, int num_threads);
void assign_threads_method4(thread_information* thread_infos, int num_threads);
void assign_threads_method5(thread_information* thread_infos, int num_threads, int offset_a, int offset_b);
void assign_threads_method6(thread_information* thread_infos, int num_threads, int num_lower_starting_threads,
    int lower_start_x, int lower_start_y);

int main(int argc, char** argv){
  srand(time(NULL));
  int thread_assignment_method = 1;
  int answer;
  bool in_testing_mode;



  /*
   * Choose either Testing Mode or Experimentation Mode
   */
  if (argc > 1){
    if (atoi(argv[1]) == 1){
      if (argc!=4){
        cout << endl;
        cout << "Usage (testing mode): ./dp_mem (in_testing_mode = 1) (size) ";
        cout << "(num_threads)";
        cout << endl << endl;
        exit(0);
      }
      in_testing_mode = true;
      num_runs = 5;
      size = atoi(argv[2]);
      num_threads = atoi(argv[3]);
      display_stats = false;

    } else if (atoi(argv[1]) == 0){
      if (argc!=7){
        cout << endl;
        cout << "Usage (experimentation mode): ./dp_mem (in_testing_mode = 0) ";
        cout << "(size) (num_threads) (thread_assignment_method) (num_runs) ";
        cout << "(display_stats) ";
        cout << endl << endl;
        exit(0);
      }
      in_testing_mode = false;
      size = atoi(argv[2]);
      num_threads = atoi(argv[3]);
      thread_assignment_method = atoi(argv[4]);
      num_runs = atoi(argv[5]);
      if (atoi(argv[6]) == 0){
        display_stats = false;
      } else if (atoi(argv[6]) == 1){
        display_stats = true;
      } else {
        cout << endl;
        cout << "Sixth argument (display_stats) must be 0 (don't display) ";
        cout <<  "or 1 (do display)";
        cout << endl << endl;
        exit(0);
      }



    } else {
      cout << endl;
      cout << "First argument (in_testing_mode) must be 0 (use testing mode) ";
      cout << "or 1 (use experiment mode) ";
      cout << endl << endl;
      exit(0);
    }



    /*
     * Generate Random Sequences A,B to find LCS of
     */
    A = genRandomString(size);
    B = genRandomString(size);




    /*
     * Testing Mode
     */
    if (in_testing_mode == true){
      check_many(num_runs);
    }




    /*
     * Experimentation Mode
     */
    if (in_testing_mode == false){
      //cout << "Num_threads, size, secs, AVG_already_solved, "
      //<< "SD_already_solved, "
      //<< "AVG_iterations, "
      //<< "SD_iterations"
      //<< endl;
      int original_number_threads = num_threads;
      for (num_threads = 1; num_threads <= original_number_threads; num_threads++){
        for (int run = 0; run < num_runs; run++){
          lcs(thread_assignment_method);
        }
      }
    }
  }
  else{
    cout << "Usage (experimentation mode): ./dp_mem (mode) ";
    cout << "(size) (num_threads) (thread_assignment_method if mode 0)";
    cout << "(num_runs if mode 0) (display_stats if mode 0) ";
  }



  return 0;
}


/*
 * For fixed sequences A and B, run LCS multiple times, varying over number threads
 */
void check_many(int num_runs){
  int thread_assigment_method = 1;

  for (int n_t = 1; n_t <= num_threads; n_t++){
    cout << "NUM THREADS IS: " << n_t << endl;

    for (int i = 0; i < num_runs; i++){
      int answer = lcs(thread_assigment_method);
      cout << "LCS determined: " << answer << " as common length" << endl;
    }
    cout << endl << "==========================================" << endl;
  }
}




/*
 * Assign ALL threads to start at the same original problem;
 */
void assign_threads_method1(thread_information* thread_infos, int num_threads){
  for (int tid = 0; tid < num_threads; tid++){
    thread_infos[tid].start_x = a -1;
    thread_infos[tid].start_y = b -1;
  }
}


/*
 * Assign ONE thread to start at the original problem, and the remaining threads to
 * to start at subproblems a few recursive levels below the original problem
 */
void assign_threads_method2(thread_information* thread_infos, int num_threads){
  for (int tid = 0; tid < num_threads-1; tid++){
    thread_infos[tid].start_x = a - (tid + 1);
    thread_infos[tid].start_y = b - ((num_threads-1) - tid );
  }
  thread_infos[num_threads-1].start_x = a - 1;
  thread_infos[num_threads-1].start_y = b - 1;
}

/*
 * Assign ONE thread to the original problem, and all remaining threaeds to
 * completely arbitrary subproblems,
 */
void assign_threads_method3(thread_information* thread_infos, int num_threads){
  for (int tid = 0; tid < num_threads-1; tid++){
    thread_infos[tid].start_x = rand()%a ;
    thread_infos[tid].start_y = rand()%b ;
  }
  thread_infos[num_threads-1].start_x = a - 1;
  thread_infos[num_threads-1].start_y = b - 1;
}


/*
 * Assign ONE thread to the original problem, and all remaining threads evenly
 * to subproblems on the the diagonal of the table.
 */
void assign_threads_method4(thread_information* thread_infos, int num_threads){
  for (int tid = 0; tid < num_threads-1; tid++){
    thread_infos[tid].start_x = (int) (   (a - 1) - tid*(a/num_threads-1)   );
    thread_infos[tid].start_y = (int) (   (b - 1) - tid*(b/num_threads-1)   );
  }
  thread_infos[num_threads-1].start_x = a - 1;
  thread_infos[num_threads-1].start_y = b - 1;
}

/*
 * Assign ONE thread to the original problem, and all remaining threads to random
 * subproblems near the original problem;
 */
void assign_threads_method5(thread_information* thread_infos, int num_threads,
    int offset_a, int offset_b){

  for (int tid = 0; tid < num_threads-1; tid++){
    thread_infos[tid].start_x = a - (tid + 1) - rand()%offset_a;
    thread_infos[tid].start_y = b - ((num_threads-1) - tid )  - rand()%offset_b;
  }
  thread_infos[num_threads-1].start_x = a - 1;
  thread_infos[num_threads-1].start_y = b - 1;

}

/*
 * Assign a portion of threads to subproblems close to the end, and the rest to
 * the original subproblem
 */
void assign_threads_method6(thread_information* thread_infos, int num_threads,
    int num_lower_starting_threads,
    int lower_start_x, int lower_start_y){

  /*
   * Starting position for shorter problem (closer to base case)
   */
  for (int tid = 0; tid < num_lower_starting_threads; tid++){
    thread_infos[tid].start_x = lower_start_x;
    thread_infos[tid].start_y = lower_start_y;
  }

  /*
   * Starting position for original problem
   */
  for (int tid = num_lower_starting_threads; tid < num_threads; tid++){
    thread_infos[tid].start_x = a-1;
    thread_infos[tid].start_y = b-1;
  }
}



/*
 * Figure out the subproblems our current problem is dependent on; also reports
 * back the number of dependent subproblems
 */
void get_dependent_lcs_subproblems(problem* prob,int* num_subprobs, problem*
    subprobs){

  int x,y;
  x = prob->x;
  y = prob->y;
  *num_subprobs = 0;


  /*
   * If current problem is LCS(A[0],B[0]), then no subproblems (hit base case)
   */
  if ((x==0)&&(y==0)){
    return;
  }


  /*
   * If string B is length 1, then LCS(A[:x],B[:y]) depends on
   * LCS(A[:x-1], B[:y]) , and opposite when A is length 1
   */
  if (y==0){
    if (table[x-1][y]==-1){
      subprobs[0].x = x-1;
      subprobs[0].y = y;
      *num_subprobs=1;
    }
    return;
  } else if (x==0){
    if (table[x][y-1]==-1){
      subprobs[0].x = x;
      subprobs[0].y = y-1;
      *num_subprobs=1;
    }
    return;
  }

  /*
   * This code is commented out because it represents an attempt to optimize
   * Randomized Subproblems that didn't quite work; namely, we wanted to avoid
   * pushing problems onto the stack if they have already been solved.
   * Oddly enough, adding this optimization, although conceptually correctly,
   * drastically messes up our LCS solver (gives very wrong answers)
   * We left this in as a demonstration of intent / future work to do

   if (y==0){
   *num_subprobs = 1;
   subprobs[0].x = x-1;
   subprobs[0].y = y;
   return;
   } else if (x==0){
   *num_subprobs = 1;
   subprobs[0].x = x;
   subprobs[0].y = y-1;
   return;
   }
   */


  /*
   * If A[a-x] != B[b-y], then LCS(A[:x],B[:y]) depends on
   * LCS(A[:x-1],B[:y]) & LCS(A[:x],B[:y-1])
   */
  if (A[x]!=B[y]){
    *num_subprobs = 2;
    subprobs[0].x = x-1;
    subprobs[0].y = y;
    subprobs[1].x = x;
    subprobs[1].y = y-1;
  }



  /*
   * This code is commented out because it represents an attempt to optimize
   * Randomized Subproblems that didn't quite work; namely, we wanted to avoid
   * pushing problems onto the stack if they have already been solved.
   * Oddly enough, adding this optimization, although conceptually correctly,
   * drastically messes up our LCS solver (gives very wrong answers)
   * We left this in as a demonstration of intent / future work to do

   if (A[x]!=B[y]){

   if (table[x-1][y] == -1){
   subprobs[0].x = x-1;
   subprobs[0].y = y;
//cout << "("<<x << ", " << y <<") ==> ("<<subprobs[0].x<<", ";
//cout <<subprobs[0].y<<")" << endl;
   *num_subprobs=1;
   }
   if (table[x][y-1]==-1){
   subprobs[(*num_subprobs)].x = x;
   subprobs[(*num_subprobs)].y = y-1;
//cout << "("<<x << ", " << y <<") ==> ("<<subprobs[(*num_subprobs)].x;
//cout << ", "<<subprobs[(*num_subprobs)].y<<")" << endl;
   *num_subprobs+=1;
   }
//    cout << "*num_subprobs: "<< *num_subprobs << endl;
return;
}
*/



/*
 * If A[x] == B[y], then LCS(A[:x],B[:y]) and
 * LCS(A[:x-1],B[:y-1])
 */
if (A[x]==B[y]){
  *num_subprobs = 1;
  subprobs[0].x = x-1;
  subprobs[0].y = y-1;
  return;
}



}


/*
 * Sets up a given thread to work on Randomized Subproblems LCS Algorithm
 */
static void*  lcs_helper2(void* thread_data){


  /*
   * Thread-relevant variables
   */
  struct thread_information* thread_info;
  int tid;


  /*
   * Variables for iterative memoization solution
   */
  Stack< problem >* S;
  problem intial_problem, curr_problem;
  int prob_x,prob_y,subprob_x,subprob_y;
  int num_subprobs;
  problem* subprobs;
  bool subproblems_all_solved;
  int* random_stream;
  int random_stream_size;
  int num_iters;
  int random_stream_position;


  /*
   * Re-cast thread's struct as thread_information
   */
  thread_info = (struct thread_information*) thread_data;
  tid = thread_info-> tid;
  random_stream = thread_info->random_stream;
  random_stream_size = thread_info->random_stream_size;
  random_stream_position = thread_info->random_stream_position;
  srand(time(NULL)*((tid+2)^tid));




  /*
   * Initialize Stack for iterative memoization + Subproblems storage
   */
  S = new ArrayStack< problem >();
  subprobs = new problem[2];
  num_subprobs = 0;
  num_iters = 0;




  /*
   * Push initial problem onto stack
   */
  intial_problem.x = thread_info->start_x;
  intial_problem.y = thread_info->start_y;
  S->push(intial_problem);


  /*
   * Iterative Memoization for LCS
   */
  while (!S->isEmpty()){
    num_iters+=1;

    /*
     * If original problem has already been solved, nothing more to be done
     */
    if (table[intial_problem.x][intial_problem.y] != -1){
      break;
    }

    curr_problem = S->getTop();
    prob_x = curr_problem.x;
    prob_y = curr_problem.y;

    /*
       if  (tid ==0 ){
       printTable(table,a,b,prob_x,prob_y);
       }
       */

    /*
     * Hit base case, empty string
     */
    if ((prob_x == -1)||(prob_y == -1)){
      S->pop();
    }

    /*
     * Problem is solved, remove problem from stack
     */
    else if (table[prob_x][prob_y] != -1){
      S->pop();
      thread_info->times_already_solved+=1;
    }


    /*
     * If problem is NOT solved: check to see if can solve via existing solved
     * subproblems
     */
    else {

      /*
       * Get the subproblems that LCS algorithm says LCS(A[:x],B[:y]) depends on
       */
      get_dependent_lcs_subproblems(&curr_problem,&num_subprobs,subprobs);



      /*
       * Determine which subproblems are still unsolved (if any), and push them
       * onto stack
       */
      int j;
      subproblems_all_solved = true;
      for (int i = 0; i < num_subprobs; i++){
        if (num_subprobs == 2){
          if (i==0){
            j = random_stream[(num_iters+random_stream_position) %
              random_stream_size];
          } else {
            j = 1-j;
          }
        } else {
          j = 0;
        }
        if (table[subprobs[j].x][subprobs[j].y] == -1) {
          S->push(subprobs[j]);
          subproblems_all_solved = false;
        }
      }


      /*
       * If all subproblems have been solved, then compute solution to current
       * problem, and remove it from the stack
       */
      if (subproblems_all_solved == true){
        S->pop();
        if ((prob_x==0) || (prob_y==0)){

          if (A[prob_x] == B[prob_y]){
            table[prob_x][prob_y] = 1;
          } else {
            if ((prob_x==0)&&(prob_y==0)){
              table[prob_x][prob_y] = 0;
            } else if  (prob_x == 0){
              table[prob_x][prob_y] = table[prob_x][prob_y-1];
            } else if (prob_y==0){
              table[prob_x][prob_y] = table[prob_x-1][prob_y];
            }
          }
        } else {
          if (A[prob_x] == B[prob_y]){
            table[prob_x][prob_y] =  1 + table[subprobs[0].x][subprobs[0].y];
          } else{
            table[prob_x][prob_y] = max(max(table[subprobs[0].x][subprobs[0].y],
                  table[subprobs[1].x][subprobs[1].y]),0);
          }
        }
      }


    } //end of else

  } //end of while


  /*
   * Store stats + answer for thread's work;
   */
  gettimeofday(&(thread_info->end_time), NULL);
  thread_info->num_iters = num_iters;
  thread_info->answer = table[intial_problem.x][intial_problem.y];


  /*
   * Clean up memory
   */
  delete S;
  delete [] subprobs;
  void* garbage;

  return garbage;
}




/*
 * Set up threads to each preform their own, independent LCS
 */
int lcs_helper1(int num_threads, int thread_assignment_method){
  int toReturn;
  pthread_t* mythreads;
  thread_information* thread_infos;
  int* iterations;
  int* times_already_solved;

  /*
   * Make sure sequence length >= number of threads
   */
  if (num_threads >= min(a,b)){
    num_threads = min(a,b) - 1;
  }


  /*
   * If either string is zero, don't bother with LCS-- answer is zero!
   */
  if ((a==0)||(b==0)){
    return 0;
  }

  int offset_a;
  int offset_b;

  offset_a = (int) a/100;
  offset_b = (int) b/100;


  /*
   * Set up Thread info structs
   */
  int num_problems_to_solve = 2;
  int random_stream_size = 1000;
  int lower_start_x = (int) a/5;
  int lower_start_y = (int) b/5;
  int num_lower_starting_threads = 0;
  int max_normal_threads = 4;
  if (num_threads > max_normal_threads){
    num_lower_starting_threads = 1;
  }

  thread_infos = new thread_information[num_threads];

  /*
   * Generate Random Bit string to use in place of random numbers
   */
  int* random_stream = new int[random_stream_size];
  for (int i=0; i < random_stream_size; i++){
    random_stream[i] = rand()%2;
  }



  /*
   * Fill in basic thread information + choose starting positions for threads
   */
  for (int tid = 0; tid < num_threads; tid++){
    thread_infos[tid].tid = tid;
    thread_infos[tid].num_threads = num_threads;
    thread_infos[tid].answer = 0;
    thread_infos[tid].times_already_solved = 0;
    thread_infos[tid].num_iters = 0;
    thread_infos[tid].random_stream_size = random_stream_size;
    thread_infos[tid].random_stream = random_stream;
    thread_infos[tid].random_stream_position = tid*(a/num_threads);
  }

  /*
   * Choose thread assignment method
   */
  if (thread_assignment_method==1){
    assign_threads_method1(thread_infos, num_threads);
  } else if(thread_assignment_method==2){
    assign_threads_method2(thread_infos, num_threads);
  } else if(thread_assignment_method==3){
    assign_threads_method3(thread_infos, num_threads);
  } else if(thread_assignment_method==4){
    assign_threads_method4(thread_infos, num_threads);
  } else if(thread_assignment_method==5){
    assign_threads_method5(thread_infos, num_threads,offset_a,offset_b);
  } else if(thread_assignment_method==6){
    assign_threads_method6(thread_infos, num_threads, num_lower_starting_threads,
        lower_start_x, lower_start_y);
  }





  /*
   * Spawn Threads
   */
  mythreads = new pthread_t[num_threads];
  for (int tid = 0; tid < num_threads; tid++){
    pthread_create(&(mythreads[tid]), NULL, lcs_helper2,
        (void *) &thread_infos[tid]);
    sleep((double) 0.3);
  }

  /*
   * Join all threads back together
   */
  for (int tid=0; tid < num_threads; tid++){
    pthread_join(mythreads[tid], NULL);
  }





  /*
   * Gather statistics on overall thread behavior during Randomized Subproblems
   */
  if (display_stats){
    iterations = new int[num_threads];
    times_already_solved = new int[num_threads];
    for (int tid = 0; tid < num_threads; tid ++){
      iterations[tid] = thread_infos[tid].num_iters;
      times_already_solved[tid] = thread_infos[tid].times_already_solved;
    }

    /*
     * Compute AVG and VAR of:
     *   i) number of times threads encountered an already-solved problem
     *  ii) number of iterations thread ran for during Randomized Subproblems
     */
    mean_encountered_already_solved = sample_mean(times_already_solved);
    mean_iterations = sample_mean(iterations);
    var_encountered_already_solved = sample_var(times_already_solved);
    var_iterations = sample_var(iterations);

    delete [] times_already_solved;
    delete [] iterations;
  }




  /*
   * Report answer to original problem
   */
  toReturn = table[a-1][b-1];



  /*
   * Clean up memory
   */
  delete [] random_stream;
  delete [] thread_infos;
  delete [] mythreads;


  return toReturn;
}






/* lcs (string A, string B)
 * finds the longest common subsequence
 * of strings A and B
 *
 * also times the run and outputs the time
 */
int lcs (int thread_assignment_method){

  struct timeval  end;

  a = A.length();
  b = B.length();

  //cout << endl;
  //cout << "A: " << A << endl;
  //cout << "B: " << B << endl;

  //Initializing table and
  //setting all values to -1, so that previously
  //unseen values can be detected by checking if
  //the table entry is -1
  table = initTable(a, b);

  int toReturn;


  //code for time with start and end
  gettimeofday(&start, NULL);

  toReturn = lcs_helper1(num_threads, thread_assignment_method);

  gettimeofday(&end, NULL);

  //calculate difference in times
  float range = computeTime(&end);


  if (!display_stats){
    printf("%d, %lu, %f \n",num_threads,A.length(),range);
  } else {
    printf("%3d, %10lu, %8.5f, %14.4Lf, %14.4Lf, %14.4Lf, %14.4Lf \n",num_threads,
        A.length(),range, mean_encountered_already_solved,
        sqrt(var_encountered_already_solved), mean_iterations,sqrt(var_iterations));
  }



  //cleaning up table
  for (int i=0; i < a; i++){
    delete [] table[i];
  }
  delete [] table;

  return toReturn;
}

/*
 * Run basic tests for LCS implementation, where answers are known!
 */
void test(){

  int len;
  int thread_assignment_method;


  thread_assignment_method = 1;



  A = "EABC";
  B = "FBAC";

  len = lcs(thread_assignment_method);

  cout << "length found is: " << len << endl;
  cout << "length should be 2" << endl << endl;


  A = "AEIOUR";
  B = ":LKSDG";


  len = lcs(thread_assignment_method);


  cout << "length found is: " << len << endl;
  cout << "length should be 0" << endl << endl;


  A = "";
  B = ":LKSDG";

  len = lcs(thread_assignment_method);

  cout << "length found is: " << len << endl;
  cout << "length should be 0" << endl << endl;


  A = "";
  B = "";

  len = lcs(thread_assignment_method);

  cout << "length found is: " << len << endl;
  cout << "length should be 0" << endl << endl;


  A = "AEGJSDIL";
  B = "AEGJSDIL";

  len = lcs(thread_assignment_method);

  cout << "length found is: " << len << endl;
  cout << "length should be 8" << endl << endl;



  A = "KWIGLI";
  B = "KIGWLA";

  len = lcs(thread_assignment_method);

  cout << "length found is: " << len << endl;
  cout << "length should be 4" << endl << endl;

}




/*
 * Generate a random string of size length;
 */
string genRandomString(int len){

  static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";//"ABCD"

  string toReturn = string(len, '0');

  for (int i = 0; i < len; ++i) {
    toReturn[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  toReturn[len] = 0;

  return toReturn;

}

/*
 * Initialize DP table for LCS to all -1's
 */
int**  initTable(int a, int b){

  int** table = new int*[a];
  for (int i=0; i < a; i++){
    table[i] = new int[b];
    for (int j=0; j < b; j++){
      table[i][j] = -1;
    }
  }

  return table;
}

/*
 * Prints out the DP table, indexed by letters of sequences A and B
 */
void printTable(int** table, int a_len, int b_len,int curr_x,int curr_y){

  printf("  ");
  for (int i = 0; i < a_len; i++){
    printf("%4c",B[i]);
  }
  printf("\n");

  for (int i = 0; i < a_len; i++){
    for (int j = 0; j < b_len; j++){
      if (j==0){
        printf("%3c",A[i]);
      }
      if ((curr_x == i)&&(curr_y == j)){
        printf("(%2d)",table[i][j]);
      } else{
        printf("%3d ",table[i][j]);
      }

    }
    cout << endl;
  }
  cout << endl;
}


/*
 * Compute the time since Randomized Subproblems started
 */
float computeTime(struct timeval* end){
  float range = (((end->tv_sec * 1000000) + end->tv_usec) -
      ((start.tv_sec * 1000000) + start.tv_usec))/1000000.0;

  return range;
}

/*
 * Compute the sample mean for data collected by threads
 */
long double sample_mean(int* data){
  long double mean=0;
  for (int tid = 0; tid < num_threads; tid++){
    mean += data[tid];
  }
  mean = mean/num_threads;
  return(mean);
}

/*
 * Compute the sample variance for data collected by threads
 */
long double sample_var(int* data){
  long double sum_of_squares = 0;
  long double mean = sample_mean(data);
  long double sample_variance;


  if (num_threads == 1){
    return 0;
  }

  for (int tid = 0; tid < num_threads; tid++){
    sum_of_squares += (data[tid]-mean)*(data[tid]-mean);
  }

  sample_variance = sum_of_squares/(num_threads-1);

  return (sample_variance);
}
