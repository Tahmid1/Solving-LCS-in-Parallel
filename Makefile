# build an executable for each of the three sequential programs
# use ./seq_mem to run memoization
# use ./seq_tab to run tabulation
# use ./seq_seq to run non DP solution
# etc.

CC = nvcc
CFLAGS = -g -G
AR = /usr/bin/ar -rvs


all: seq_mem seq_tab seq_seq dp_wave dp_mem dp_gpu dp_gpu_visual dp_wave_vector

seq_mem: DP_sequential_memoization.cpp
	g++ -o seq_mem DP_sequential_memoization.cpp


seq_tab: DP_sequential_tabulation.cpp
	g++ -o seq_tab DP_sequential_tabulation.cpp

seq_seq: LCS_sequential.cpp
	g++ -o seq_seq LCS_sequential.cpp

par_rand_nums: DP_parallel_random_numbers.cpp
	g++-4.8 -std=c++11 DP_parallel_random_numbers.cpp -o par_rand_nums -pthread

dp_rand: DP_parallel_randomized.cpp
	g++-4.8 -std=c++11 DP_parallel_randomized.cpp -o dp_rand -pthread

dp_wave: DP_parallel_wave.cpp
	clang++ DP_parallel_wave.cpp -o dp_wave -pthread

dp_wave_vector: DP_parallel_wave_vectorized.cpp
	clang++ DP_parallel_wave_vectorized.cpp -o dp_wave_vector -pthread

dp_mem: DP_parallel_memoization.cpp
	clang++ DP_parallel_memoization.cpp -o dp_mem -pthread

	#g++-4.8 -std=c++11 DP_parallel_memoization.cpp -o dp_mem -pthread

dp_gpu: DP_parallel_GPU.cu libmyopengllib.a
	${CC}  ${CFLAGS}  -o dp_gpu DP_parallel_GPU.cu -L. -lmyopengllib  -lglut -lGL -lGLU -lGLEW

dp_gpu_visual: DP_parallel_wave_visualer.cu libmyopengllib.a
	${CC}  ${CFLAGS}  -o dp_gpu_visual DP_parallel_wave_visualer.cu -L. -lmyopengllib  -lglut -lGL -lGLU -lGLEW

myopengllib.o: myopengllib.cu
	${CC} ${CFLAGS} -c myopengllib.cu -lglut -lGL -lGLU -lGLEW

libmyopengllib.a: myopengllib.o
	${AR} libmyopengllib.a  myopengllib.o


clean:
	$(RM) dp_gpu_visual seq_mem seq_tab seq_seq dp_wave dp_mem myopengllib.o libmyopengllib.a dp_gpu dp_wave_vector
