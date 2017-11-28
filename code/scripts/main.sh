#!/bin/bash

comp=gcc

echo "=======Pre Process ====================================================================="
if [ "$comp" == "intel" ] ; then
    module load intel intelmpi/17.4
fi
if [ "$comp" == "gcc" ] ; then
    module load gcc openmpi
fi
set -x 
ipn=1;nt=4;
datapath=../data/${MTD}-${N}-${FD}-ep2.7-o4-gc-0.05/
JobID="${SLURM_JOB_ID}_P${P}_N${N}"
app=../bin/dtsw_${postfix}
params="-P $P -p $p -q $q -M $N $B $b -N $N $B $b -t $nt --ipn $ipn --data-path $datapath --iter-no $iter --timeout 300"
outfile=$tempdir/run_${JobID}.out

echo "=======Process==========================================================================="
#srun  -n $P --ntasks-per-node=$ipn -m cyclic:cyclic:* -l --output $outfile $app ${params}
mpirun -n $P --map-by ppr:1:node --output-filename $outfile $app ${params}


echo "=======Post Process ====================================================================="
if [ "$comp" == "intel" ] ; then
    for i in $(seq 0 $[$P-1])
    do
	grep "${i}:" $outfile >$tempdir/tests_p${i}.out
	str="s/$i://g"
	sed -i -e $str $tempdir/tests_p${i}.out
    done 
    grep "First task submitted" $tempdir/tests_p0.out >> $tempdir/timing.txt
    grep "Program finished" $tempdir/tests_p0.out >> $tempdir/timing.txt
    grep " timeout" $tempdir/tests_p0.out >> $tempdir/timing.txt
    echo -n "MTD:$MTD," >> $tempdir/timing.txt
    grep "P:" $tempdir/tests_p0.out >> $tempdir/timing.txt
fi
if [ "$comp" == "gcc" ] ; then
    i=0
    if [ $P > 10 ]; then
      i=00;
    fi 
    grep "First task submitted" ${outfile}*.$i  >> $tempdir/timing.txt
    grep "Program finished" ${outfile}*.$i >> $tempdir/timing.txt
    grep " timeout" ${outfile}*.$i$i  >> $tempdir/timing.txt
    echo -n "MTD:$MTD," >> $tempdir/timing.txt
    grep "P:" ${outfile}*.$i  >> $tempdir/timing.txt
fi
rm *file*.txt
