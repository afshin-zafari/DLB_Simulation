#! /bin/bash

files=$(ls ./N*/*/timing*)
for f in $files
do
    echo $f
    python timings.py $f >> all_times.txt
done

grep "MTD" all_times.txt > times.txt
rm all_times.txt
cp times.txt all_times_pure_mpi.txt
rm times.txt