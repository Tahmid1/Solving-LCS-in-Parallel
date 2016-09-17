/* DP_sequential_memoization.cpp
 *   by Brandon Chow and Tahmid Rahman
 *   for CS87 final project
 * Contains code for memoization DP version
 *   of finding the longest common subsequence
 */

#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/time.h>

using namespace std;

#define SIZE 9

int lcs_helper(string A, string B, int a, int b, int** table);
int lcs(string A, string B);
string genRandom(int len);
void initTable(int** table, int a_len, int b_len);
void printTable(int** table, int a_len, int b_len);

/* lcs (string A, string B)
 * finds the longest common subsequence
 * of strings A and B
 *
 * also times the run and outputs the time
 */
int lcs (string A, string B){
  struct timeval start, end;

  int a = A.length();
  int b = B.length();

  //initializing a table to be used for DP
  int** table;
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

  //returns LCS of strings A and B
  toReturn = lcs_helper(A, B, a, b, table);

  gettimeofday(&end, NULL);

  //calculate difference in times
  float range = (((end.tv_sec * 1000000) + end.tv_usec) -
      ((start.tv_sec * 1000000) + start.tv_usec))/1000000.0;

  //printf("Time for %d iterations: %f seconds.\n", sims, range);
  //printf("Time: %f \n", range);
  printf("%d, %lu, %f \n",1,A.length(),range);

  //printTable(table, a+1, b+1);

  //cleaning up table
  for (int i=0; i < a+1; i++){
    delete [] table[i];
  }
  delete [] table;

  return toReturn;
}

/* lcs_helper (A, B, length of A, length of B, and table)
 * does the actual work of finding the LCS using memoization
 */
int lcs_helper (string A, string B, int a, int b, int** table){
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
      answer =  1 + lcs_helper(A, B, a-1, b-1, table);
    }
  }

  //A and B don't have matching character
  //so its dependent on A[0:-1] and B
  //vs
  //A and B[0:-1]
  else{
    int max_sub_len1;
    int max_sub_len2;

    //finding max_sub_len1
    //which represents the max LCS length
    //between one less character of A and
    //all of B
    if (table[a-1][b] != -1){
      max_sub_len1 = table[a-1][b];
    }
    else{
      max_sub_len1 = lcs_helper(A, B, a-1, b, table);
    }

    //finding max_sub_len2
    //which represents the max LCS length
    //between one less character of B and
    //all of A
    if (table[a][b-1] != -1){
      max_sub_len2 = table[a][b-1];
    }
    else{
      max_sub_len2 = lcs_helper(A, B, a, b-1, table);
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

//a test function to make sure implementation is working correctly
void test(){
  string A = "AEIOU";
  string B = "AOU";

  int len = lcs(A, B);

  cout << "length found is: " << len << endl;
  cout << "length should be 3" << endl << endl;

  A = "AEIOU";
  B = ":LKSDG";

  len = lcs(A, B);

  cout << "length found is: " << len << endl;
  cout << "length should be 0" << endl << endl;

  A = "";
  B = ":LKSDG";

  len = lcs(A, B);

  cout << "length found is: " << len << endl;
  cout << "length should be 0" << endl << endl;

  A = "";
  B = "";

  len = lcs(A, B);

  cout << "length found is: " << len << endl;
  cout << "length should be 0" << endl << endl;

  A = "AEGJSDIL";
  B = "AEGJSDIL";

  len = lcs(A, B);

  cout << "length found is: " << len << endl;
  cout << "length should be 8" << endl << endl;


  A = "KWIGLI";
  B = "KIGWLA";

  len = lcs(A, B);

  cout << "length found is: " << len << endl;
  cout << "length should be 4" << endl << endl;

}

//craetes a random string of length len
string genRandom(int len){

  static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  string toReturn = string(len, '0');

  for (int i = 0; i < len; ++i) {
    toReturn[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  toReturn[len] = 0;

  return toReturn;

}

int main(int argc, char** argv){
  int answer;
  int num_runs = 5;

  srand(time(NULL));
  //parse input	
  if(argc >= 2) {
    if (atoi(argv[1]) == 1){
      test();
    }
    else if (atoi(argv[1]) == 0){
      if (argc != 3){
        printf("usage: ./seq_mem (mode = 0) (size)\n");		
      }
      else{
        int size = SIZE;

        size = atoi(argv[2]);

        string A;
        string B;

        A = genRandom(size);
        B = genRandom(size);

        answer = lcs(A, B);				
      }
    }
  }
  else{
    printf("usage: ./seq_mem (mode) (size if mode = 0)\n");
  }
  //cout << "LCS determined: " << answer << " as common length" << endl;
  /*test();*/
}
