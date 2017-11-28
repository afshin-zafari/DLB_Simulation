#!/bin/bash
#SBATCH -A SNIC2017-7-18
#SBATCH -o tests-%j.out
#SBATCH -p node
#SBATCH -t 00:02:00
#SBATCH -N 2
#SBATCH -n 40
#SBATCH -J N04-ShallowWater

#assert (ipn * nt  == 20 )
#assert ( p  * q   == P  )
#assert ( -N * ipn == P  ) 
#assert ( -N * 20  == -n ) 
#assert (  P * nt  == -n ) 

k=1;DLB=0
P=2;p=2;q=1;
ipn=$P;nt=20;
B=8;b=2

#assert ( B >= p ) 
#assert ( B >= q )



N=6400
#====================================

module load intel intelmpi/17.4
JobID=${SLURM_JOB_ID}
app=./bin/dtsw 

params="-P $P -p $p -q $q -M $N $B $b -N $N $B $b -t $nt --ipn $ipn --timeout 10"
echo "Params: $params"
tempdir=./temp
mkdir -p $tempdir
outfile=$tempdir/tests_${JobID}.out
echo "========================================================================================="
set -x 
rm $outfile
srun  -n $P -c $nt -m cyclic:cyclic:* -l --output $outfile $app ${params}
#if [ "z$1z" == "zz" ]; then 
#	srun  -ordered-output -n $P -ppn $ipn -outfile-pattern  $outfile -errfile-pattern $outfile  -l  $app ${params}
#else
#	srun  -n $P $app ${params}
#fi
for i in 0 1 2 3 
do
    grep "${i}:" $outfile >$tempdir/tests_p${i}.out
    sed -i -e 's/$i://g' $tempdir/tests_p${i}.out
done 
 
rm *file*.txt
