# CS87 Project; Bash script for timing Randomized Subproblems
# Brandon Chow and Tahmid Rahman
# 2016-05-04

maximum_number_threads=16
num_runs=5;
displaystats=1


#echo "Num_threads, size, secs, AVG_already_solved, SD_already_solved, AVG_iterations, SD_iterations, thread_assigment_method"

#for ((thread_assigment_method=1; thread_assigment_method <=6; thread_assigment_method=thread_assigment_method+1)) do#
#  for ((sequence_length=1000; sequence_length <= 15000; sequence_length=sequence_length+1000)) do
#    ./dp_mem 0 $sequence_length $maximum_number_threads $thread_assigment_method $num_runs $displaystats
#  done
#done

#./dp_mem 0 25000 1 5 $num_runs $displaystats

for ((thread_assigment_method=1; thread_assigment_method <= 6; thread_assigment_method=thread_assigment_method+1)) do
  for ((sequence_length=25000; sequence_length <= 100000; sequence_length=sequence_length+5000)) do
#  echo "./dp_mem 0 $sequence_length $maximum_number_threads $thread_assigment_method $num_runs $displaystats"
    ./dp_mem 0 $sequence_length $maximum_number_threads $thread_assigment_method $num_runs $displaystats
  done
done
