This repo contains code made for a project
by Brandon Chow and Tahmid Rahman made in
Spring of 2016 at Swarthmore College.

It explores parallel solutions to solving
the longest common subsequence problem.

=====================
----- BUILDING ------
=====================

First, run make clean.
Then, run make to build.






=====================
-------RUNNING-------
=====================

The executables are:

seq_seq - code in LCS_sequential.cpp

seq_mem - code in DP_sequential_memoization.cpp

seq_tab - code in DP_sequential_tabulation.cpp

dp_mem - code in DP_parallel_memoization.cpp

dp_wave - code in DP_parallel_wave.cpp

dp_wave_vector - code in DP_parallel_wave_vectorized.cpp

dp_gpu - code in DP_parallel_GPU.cu

dp_gpu_visual - code in DP_parallel_wave_visualer.cu


ALL EXECUTABLES EXCEPT FOR 
dp_gpu AND dp_gpu_visual 
and seq_seq HAVE TWO MODES

0 - Experimentation Mode:
	In this mode, we run 
	individual tests to 
	gather data

1 - In this mode, we verify
    the correctness of our 
    code

* This mode is taken as 
the first argument 

---------------------

Running seq_seq

./seq_seq <sequence length>

e.g. ./seq_seq 8

---------------------

Running seq_mem

./seq_mem <mode> <sequence length>

e.g. ./seq_mem 0 1000

---------------------

Running seq_tab

./seq_tab <mode> <sequence length>

e.g. ./seq_tab 0 1000

---------------------

Running dp_mem

./dp_mem 0 <sequence length> <maximum number threads> <thread assigment method> <num runs> <displaystats>

e.g. ./dp_mem 0 1000 5 1 4 0

OR

./dp_mem 1 <sequence length> <num threads>

e.g. ./dp_mem 1 1000 5

---------------------

dp_wave and dp_wave_vector

./dp_wave <mode> <sequence_length> <num_threads>

e.g. ./dp_wave 0 1000 4

AND

./dp_wave_vector <mode> <sequence length> <num threads>

e.g. ./dp_wave_vector 0 1000 4

---------------------

dp_gpu and dp_gpu_visual

./dp_gpu <sequence length> 

for non visual, a window will pop up and it will appear frozen, 
but really it's waiting for the user to press enter in the 
terminal screen behind it - just move the window aside and 
the timing should be present 







