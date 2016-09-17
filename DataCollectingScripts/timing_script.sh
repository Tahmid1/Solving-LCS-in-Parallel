# CS87 Project; Bash script for getting timing data
# Brandon Chow and Tahmid Rahman
# 2016-04-22

#echo "First argument was {$var1}"




# First argument: maximum number of threads
maximum_number_threads=$1
# Second argument is: amount to increment number of threads by
increment_number_threads_by=$2


# Third argument is: maximum sequence length
maximum_length_sequence=$3
# Fourth argument is: increment length of sequences by
increment_length_sequence_by=$4

# Fifth argument is: number of iterations per choice of parameters
max_iters=$5


#
# Run a given parallel LCS program
#
get_parallel_lcs_data() {
  maximum_number_threads=$1
  increment_number_threads_by=$2
  maximum_length_sequence=$3
  increment_length_sequence_by=$4
  max_iters=$5
  executable_name=$6

  echo ""
  echo ""
  echo ""
  echo "$executable_name"
  echo "NUM_THREADS, SEQUENCE_LENGTH, SECS"

  for ((num_threads=1; num_threads <= maximum_number_threads; num_threads=num_threads+increment_number_threads_by)) do
    for ((sequence_length=1000; sequence_length <= maximum_length_sequence; sequence_length=sequence_length+increment_length_sequence_by)) do
      #echo ""
      #echo "./$executable_name $sequence_length $num_threads"
      for ((num_iters=1; num_iters <= max_iters; num_iters=num_iters+1)) do

        ./$executable_name $sequence_length $num_threads
      done
    done
  done
}


#
# Run a given sequential LCS  program
#
get_sequential_lcs_data() {

  maximum_length_sequence=$1
  increment_length_sequence_by=$2
  max_iters=$3
  executable_name=$4

  echo ""
  echo ""
  echo ""
  echo "$executable_name"
  echo "NUM_THREADS, SEQUENCE_LENGTH, SECS"


  for ((sequence_length=1000; sequence_length <= maximum_length_sequence; sequence_length=sequence_length+increment_length_sequence_by)) do
    for ((num_iters=1; num_iters <= max_iters; num_iters=num_iters+1)) do
      ./$executable_name $sequence_length
    done
  done

}


#executable_name="seq_tab"
#get_sequential_lcs_data  $maximum_length_sequence $increment_length_sequence_by $max_iters $executable_name

#executable_name="seq_mem"
#get_sequential_lcs_data  $maximum_length_sequence $increment_length_sequence_by $max_iters $executable_name

#executable_name="dp_wave"
#get_parallel_lcs_data $maximum_number_threads $increment_number_threads_by $maximum_length_sequence $increment_length_sequence_by $max_iters $executable_name

executable_name="dp_mem"
get_parallel_lcs_data $maximum_number_threads $increment_number_threads_by $maximum_length_sequence $increment_length_sequence_by $max_iters $executable_name

#executable_name="dp_gpu"
#get_sequential_lcs_data $maximum_length_sequence $increment_length_sequence_by $max_iters $executable_name
