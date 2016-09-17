# CS87 Project; Bash script for timing Wave Front
# Brandon Chow and Tahmid Rahman
# 2016-05-05


maximum_length_sequence=5000
thread_assigment_method=1
maximum_number_threads=5


for ((sequence_length=1000; sequence_length <= maximum_length_sequence; sequence_length=sequence_length+1000)) do
  for ((num_threads=1; num_threads <= maximum_number_threads; num_threads=num_threads+1)) do
    echo ""
    echo "sequence_length: $sequence_length, num_threads: $num_threads"
    ./dp_wave 0 $sequence_length $num_threads
  done
done
