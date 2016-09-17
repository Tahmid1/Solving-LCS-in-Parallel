# CS87 Project; Bash script for timing Sequential Memoization
# Brandon Chow and Tahmid Rahman
# 2016-05-04


maximum_length_sequence=3000
echo ""
echo ""
echo "Num Threads, Size, Seconds"
for ((sequence_length=1000; sequence_length <= maximum_length_sequence; sequence_length=sequence_length+1000)) do
  ./seq_mem $sequence_length
done
echo ""
echo ""
