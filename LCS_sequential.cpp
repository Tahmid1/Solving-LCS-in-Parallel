/* DP_sequential_tabulation.cpp
 *   by Brandon Chow and Tahmid Rahman
 *   for CS87 final project
 *   Contains code for sequential non-DP version
 *   of finding the longest common subsequence
 */
 
#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/time.h>

using namespace	std;

#define SIZE 9

string generate_random_sequence(int len);
int LCS_sequential_ndp(string A, string B);

int main(int argc, char** argv){
	struct timeval start, end;
	int answer;

	srand(time(NULL));
	//parse input
	if(argc > 2) {
		printf("usage: ./seq_mem (size)\n");
		exit(0);
	}
	int size = SIZE;
	if(argc == 2) {
		size = atoi(argv[1]);
	}

	string A;
	string B;

	//A and B are random strings of length size
	A = generate_random_sequence(size);
	B = generate_random_sequence(size);

	//start and end are useful for getting timing
	gettimeofday(&start, NULL);

	answer = LCS_sequential_ndp(A,B);

	gettimeofday(&end, NULL);

	//calculate difference in times
  	float range = (((end.tv_sec * 1000000) + end.tv_usec) - 
  		((start.tv_sec * 1000000) + start.tv_usec))/1000000.0;

  	//printf("Time for %d iterations: %f seconds.\n", sims, range);
  	printf("Time: %f \n", range);

  	cout << "LCS determined: " << answer << " as common length" << endl;


	return 0;
}

//craetes a random string of length len
string generate_random_sequence(int len){

    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    string s = string(len, '0');

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;

    return s;

}

//performs LCS, without table (just recursively)
int LCS_sequential_ndp(string A, string B){

	//A is empty or B is empty
	if ((A.length() == 0 ) || (B.length()==0)) {
		return 0;
	}

	//A's first char matches B's first char
	if (A[0] == B[0]){
		return 1 + LCS_sequential_ndp(A.substr(1), B.substr(1));
	}
	//chars don't match up
	return max(LCS_sequential_ndp(A,B.substr(1)), LCS_sequential_ndp(A.substr(1),B));

}
