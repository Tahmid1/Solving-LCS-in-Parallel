# CS87 Project; Bash script for timing Vectorized Wave Front
# Brandon Chow and Tahmid Rahman
# 2016-05-05



maximum_number_threads=16
#echo "Num_threads, sequence_length, time_after_state_init, full_run_time"


#for ((sequence_length=1000; sequence_length <= 15000; sequence_length=sequence_length+1000)) do
#  for ((num_threads=1; num_threads <= maximum_number_threads; num_threads=num_threads*2)) do
#    ./dp_wave_vector 0 $sequence_length $num_threads
#  done
#done

for ((num_runs = 1; num_runs <= 5; num_runs=num_runs+1)) do 
  for ((sequence_length=20000; sequence_length <= 50000; sequence_length=sequence_length+5000)) do
    for ((num_threads=1; num_threads <= maximum_number_threads; num_threads=num_threads*2)) do
      ./dp_wave_vector 0 $sequence_length $num_threads
    done
  done
done 
