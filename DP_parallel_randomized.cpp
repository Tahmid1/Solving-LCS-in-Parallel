#include <pthread.h>

#include <random>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/time.h>
#include <vector>
#include <algorithm>

using namespace std;

#define SIZE 9
#define NUM_THREADS 4

bool FINISHED = false;
int TID_FINISHED = -1;

pthread_mutex_t* gate;
pthread_t* mythreads;
int** table;

struct thread_information{
	int tid;
	int num_threads;
	string* A;
	string* B;
	int sizeA;
	int sizeB;
  int answer;
};




string genRandomString(int len);
void initTable(int** table, int a_len, int b_len);
void printTable(int** table, int a_len, int b_len);

int lcs (string A, string B, int num_threads);
int lcs_helper1(string A, string B, int a, int b, int** table, int num_threads);
static void*  lcs_helper2(void* thread_data);
int lcs_helper3(string A, string B, int a, int b, int tid, vector<int>* recursions, int recursive_level);
void test(int num_threads);




int main(int argc, char** argv){
  srand(time(0)); //srand(time(NULL));

  int size, num_threads;
  int answer;
  string A,B;

  cout << endl;
  cout << "**************************************************" << endl;
  cout << "*  WARNING: NEED TO MAKE THREAD-SAFE RANDOM NUMBERS" << endl;
  cout << "*  But, for some reason, they look thread safe... "<< endl;
  cout << "*  ... see DP_parallel_random_numbers.cpp !!!!!" << endl;
  cout << "**************************************************" << endl;
  cout << endl;

  size = SIZE;
  num_threads = NUM_THREADS;


  //parse input
  if (argc != 1){
    if( argc != 3)  {
  		printf("usage: ./dp_rand (size) (num_threads)\n");
  		exit(0);
  	} else {
      size = atoi(argv[1]);
      num_threads = atoi(argv[2]);
    }
  }

  //cout << "Size: "<<size << endl;
  //cout << "Number Threads: "<< num_threads << endl;

	A = genRandomString(size);
	B = genRandomString(size);

	answer = lcs(A, B, num_threads);


	cout << "LCS determined: " << answer << " as common length" << endl << endl;

  //cout << endl << endl << endl << endl;
  //test(num_threads);

  return 0;
}



//craetes a random string of length len
string genRandomString(int len){

	static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  string toReturn = string(len, '0');

  for (int i = 0; i < len; ++i) {
      toReturn[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  toReturn[len] = 0;

	return toReturn;

}


//set all table cells to be -1 at start
void initTable(int** table, int a_len, int b_len){
	for (int i = 0; i < a_len; i++){
		for (int j = 0; j < b_len; j++){
			table[i][j] = -1;
		}
	}
}


//prints out table, useful for debugging
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


void test(int num_threads){
	string A = "AEIOU";
	string B = "AOU";

	int len = lcs(A, B, num_threads);

	cout << "length found is: " << len << endl;
	cout << "length should be 3" << endl << endl;

	A = "AEIOU";
	B = ":LKSDG";

	len = lcs(A, B, num_threads);

	cout << "length found is: " << len << endl;
	cout << "length should be 0" << endl << endl;

	A = "";
	B = ":LKSDG";

	len = lcs(A, B, num_threads);

	cout << "length found is: " << len << endl;
	cout << "length should be 0" << endl << endl;

	A = "";
	B = "";

	len = lcs(A, B, num_threads);

	cout << "length found is: " << len << endl;
	cout << "length should be 0" << endl << endl;

	A = "AEGJSDIL";
	B = "AEGJSDIL";

	len = lcs(A, B, num_threads);

	cout << "length found is: " << len << endl;
	cout << "length should be 8" << endl << endl;


	A = "KWIGLI";
	B = "KIGWLA";

	len = lcs(A, B, num_threads);

	cout << "length found is: " << len << endl;
	cout << "length should be 4" << endl << endl;

}


/* lcs (string A, string B)
 * finds the longest common subsequence
 * of strings A and B
 *
 * also times the run and outputs the time
 */
int lcs (string A, string B, int num_threads){
	struct timeval start, end;

	int a = A.length();
	int b = B.length();

	//initializing a table to be used for DP
	//int** table;
	table = new int*[a+1];
	for (int i=0; i < a+1; i++){
		table[i] = new int[b+1];
	}

	//setting all values to -1, so that previously
	//unseen values can be detected by checking if
	//the table entry is -1
	initTable(table, a+1, b+1);

	int toReturn;

	//code for time with start and end
	gettimeofday(&start, NULL);

  toReturn = lcs_helper1(A,B,a,b,table,num_threads);

	gettimeofday(&end, NULL);

	//calculate difference in times
  float range = (((end.tv_sec * 1000000) + end.tv_usec) -
  	((start.tv_sec * 1000000) + start.tv_usec))/1000000.0;

  //printf("Time for %d iterations: %f seconds.\n", sims, range);
  printf("Time: %f \n", range);

	//printTable(table, a+1, b+1);

	//cleaning up table
	for (int i=0; i < a+1; i++){
		delete [] table[i];
	}
	delete [] table;

	return toReturn;
}



/*
* Spawns threads, each of which work on a randomized subproblems LCS
*/
int lcs_helper1(string A, string B, int a, int b, int** table, int num_threads){

  int* answers;
  int toReturn;
  thread_information* thread_infos;


  thread_infos = new thread_information[num_threads];
  mythreads = new pthread_t[num_threads];

	gate = new pthread_mutex_t;
	pthread_mutex_init(gate,NULL);

	//spawn off the threads
	for (int tid=0; tid < num_threads; tid++){
		thread_infos[tid].tid = tid;
		thread_infos[tid].num_threads = num_threads;
		thread_infos[tid].A = &A;
		thread_infos[tid].B  = &B;
		thread_infos[tid].sizeA = a;
		thread_infos[tid].sizeB = b;
		thread_infos[tid].answer = 0;
		pthread_create(&(mythreads[tid]), NULL, lcs_helper2, (void *) &thread_infos[tid]);
	}

	//join all threads back together
	for (int tid=0; tid < num_threads; tid++){
		pthread_join(mythreads[tid], NULL);
	}







  toReturn = thread_infos[TID_FINISHED].answer;



  delete [] thread_infos;
  delete [] mythreads;
	delete gate;
  return toReturn;
}




/*
* Calls a non-void function for the LCS solver, since threads require void functions
*/
static void*  lcs_helper2(void* thread_data){

	float range;
	struct timeval start, end;
  struct thread_information* thread_info = (struct thread_information*) thread_data;
  vector<int>* recursions = new vector<int>();



  string A = *(thread_info->A);
  string B = *(thread_info->B);
  int a = thread_info->sizeA;
  int b = thread_info->sizeA;
  int tid = thread_info->tid;
	int num_threads = thread_info->num_threads;

	srand((int)time(NULL) ^ (tid+231)^2);




  gettimeofday(&start, NULL);
	pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  thread_info->answer = lcs_helper3(A,B,a,b,tid,recursions,0);

	pthread_mutex_lock(gate);
  if ((FINISHED==false) && (TID_FINISHED==-1)){
    TID_FINISHED = tid;
    FINISHED = true;
		for (int i=0; i < num_threads; i++){
			if (i!=tid){
				pthread_cancel(mythreads[i]);
			}
		}
  }
	pthread_mutex_unlock(gate);


  gettimeofday(&end, NULL);



  //calculate difference in times
	range = (((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec))/1000000.0;
  printf("Thread %d: %f (secs) %d (num levels)  \n", tid, range,(int) recursions->size());




  delete recursions;
}



/*
* Does the actual LCS problem
*/
int lcs_helper3(string A, string B, int a, int b, int tid, vector<int>* recursions,int recursive_level){
	pthread_testcancel();


  recursions->push_back(recursive_level);

  if (FINISHED){
		return 0;
  }


  int answer;

  //string A is empty
  if (a == 0){
    table[a][b] = 0;
    return 0;
  }

  //string B is empty
  else if (b == 0){
    table[a][b] = 0;
    return 0;
  }

  //A and B have a matching character
  if (A[a-1] == B[b-1]){
    //checking if the previous value has been calculated
    //if so, just pull it out from table
    if (table[a-1][b-1] != -1){
      table[a][b] = 1 + table[a-1][b-1];
      answer = 1 + table[a-1][b-1];
    }
    //if not, then need to recursively find it
    else{
      answer =  1 + lcs_helper3(A, B, a-1, b-1,tid,recursions,recursive_level+1);
    }
  }

  //A and B don't have matching character
  //so its dependent on A[0:-1] and B
  //vs
  //A and B[0:-1]
  else{
    int max_sub_len1;
    int max_sub_len2;

    bool inside_table1 = false;
    bool inside_table2 = false;


    // See if  LCS(A[:a-1],B) already solved; if so, use answer.
    if (table[a-1][b] != -1){
      inside_table1 = true;
      max_sub_len1 = table[a-1][b];
    }

    // See if LCS(A,B[:b-1]) already solved; if so, user answer.
    if (table[a][b-1] != -1){
      inside_table2 = true;
      max_sub_len2 = table[a][b-1];
    }

    // If only one of LCS(A,B[:b-1]) and LCS(A[:a-1],B) are unsolved,
    // solve it (no random choice)
    if ((inside_table1==false) && (inside_table2==true)){
      max_sub_len1 = lcs_helper3(A, B, a-1, b,tid,recursions,recursive_level+1);

    } else if ((inside_table1==true) && (inside_table2==false)){
      max_sub_len2 = lcs_helper3(A, B, a, b-1,tid,recursions,recursive_level+1);

    } else if ((inside_table1==false) && (inside_table2==false)){

      // If both LCS(A,B[:b-1]) and LCS(A[:a-1],B) unsolved,
      // randomly choose one to solve first:
      int random_number = rand() % 2; //get random number: either 0 and 1

      //string msg = "Thread " + to_string(tid) + ": " + to_string(random_number)+ "\n";
      //cout << msg;

      if (random_number == 0){
        max_sub_len1 = lcs_helper3(A, B, a-1, b,tid,recursions,recursive_level+1);
        if (table[a][b-1] != -1){
          max_sub_len2 = table[a][b-1];
        } else{
          max_sub_len2 = lcs_helper3(A, B, a, b-1,tid,recursions,recursive_level+1);
        }
      } else {
        max_sub_len2 = lcs_helper3(A, B, a, b-1,tid,recursions,recursive_level+1);
        if (table[a-1][b] != -1){
          max_sub_len1 =  table[a-1][b];
        } else{
          max_sub_len1 = lcs_helper3(A, B, a-1, b,tid,recursions,recursive_level+1);
        }
      }
    }


    //finding the larger of max_sub_lens
    if (max_sub_len1 > max_sub_len2){
      answer =  max_sub_len1;
    }
    else{
      answer =  max_sub_len2;
    }


  }

  //update table, and return answer
  table[a][b] = answer;
  return answer;
}
