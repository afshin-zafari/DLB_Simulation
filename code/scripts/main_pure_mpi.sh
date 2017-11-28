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
ipn=$P;nt=1;

datapath=../../data/${MTD}-${N}-${FD}-ep2.7-o4-gc-0.05/
JobID="${SLURM_JOB_ID}_P${P}_N${N}"
app=../../bin/dtsw_${postfix}
params="-P $P -p $p -q $q -M $N $B $b -N $N $B $b -t $nt --ipn $ipn --pure-mpi --data-path $datapath --iter-no $iter --timeout 1000"
outfile=$tempdir/run_${JobID}.out

echo "=======Process==========================================================================="
date
if [ "$comp" == "intel" ] ; then
    srun  -n $P -c 1 --ntasks-per-node=20 -m cyclic:cyclic:* -l --output $outfile $app ${params}
else
    mpirun -n $P --map-by ppr:20:node --output-filename $outfile $app ${params}
fi
date

echo "=======Post Process ====================================================================="
if [ "$comp" == "intel" ] ; then 
    for i in $(seq 0 $[$P-1])
    do
	if [ $i < 10 ]; then  
            i="0$i"
	fi
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
    grep "First task submitted" ${outfile}*.00* >> $tempdir/timing.txt
    grep "Program finished" ${outfile}*.00* >> $tempdir/timing.txt
    grep " timeout" ${outfile}*.00* >> $tempdir/timing.txt
    echo -n "MTD:$MTD," >> $tempdir/timing.txt
    grep "P:" ${outfile}*.00* >> $tempdir/timing.txt
fi
rm *file*.txt
