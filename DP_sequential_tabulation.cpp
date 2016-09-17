/* DP_sequential_tabulation.cpp
 *   by Brandon Chow and Tahmid Rahman
 *   for CS87 final project
 *   Contains code for tabulation DP version
 *   of finding the longest common subsequence
 */

#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/time.h>

using namespace std;

#define SIZE 9

int lcs_helper(string A, string B, int a, int b);
int lcs(string A, string B);
string genRandom(int len);

/* lcs (string A, string B)
 * finds the longest common subsequence
 * of strings A and B
 *
 * also times the run and outputs the time
 */
int lcs(string A, string B){
  int toReturn;

  struct timeval start, end;

  gettimeofday(&start, NULL);

  toReturn = lcs_helper(A, B, A.length(), B.length());

  gettimeofday(&end, NULL);

  //calculate difference in times
  float range = (((end.tv_sec * 1000000) + end.tv_usec) -
      ((start.tv_sec * 1000000) + start.tv_usec))/1000000.0;

  //printf("Time for %d iterations: %f seconds.\n", sims, range);
  //printf("Time: %f \n", range);
  printf("%d, %lu, %f \n",1,A.length(),range);

  return toReturn;
}

/* lcs_helper (A, B, length of A, length of B, and table)
 * does the actual work of finding the LCS using tabulation
 */
int lcs_helper(string A, string B, int a, int b){

  //setting up table
  int** table;
  table = new int*[a+1];
  for (int i=0; i < a+1; i++){
    table[i] = new int[b+1];
  }

  //looping through table, row by row
  for (int i = 0; i <= a; i++){
    for (int j = 0; j<= b; j++){
      //we can always refer to old table values
      //because with tabulation, we are building
      //up and have always solved smaller subproblems

      //string A is empty
      if (i == 0){
        table[i][j] = 0;
      }

      //string B is empty
      else if (j == 0){
        table[i][j] = 0;
      }

      //A and B have a matching character
      else if (A[i-1] == B[j-1]){
        table[i][j] = table[i-1][j-1] + 1;
      }

      //A and B don't have a matching character
      else{
        int max_sublen1 = table[i-1][j];
        int max_sublen2 = table[i][j-1];

        //finding the max of the two sublens
        if (max_sublen1 > max_sublen2){
          table[i][j] = max_sublen1;
        }
        else{
          table[i][j] = max_sublen2;
        }
      }
    }
  }

  int toReturn = table[a][b];

  //cleaning up table
  for (int i=0; i < a+1; i++){
    delete [] table[i];
  }
  delete [] table;

  //returning the answer
  return toReturn;
}

//test function to verify correctness of DP program
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
}
