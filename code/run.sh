#!/bin/bash
#SBATCH -A SNIC2017-7-18
#SBATCH -o tests-%j.out
#SBATCH -p devel
#SBATCH -t 00:05:00
#SBATCH -N 1
#SBATCH -n 20
#SBATCH -J N01-DLBSIM

#assert (ipn * nt  == 20 )
#assert ( p  * q   == P  )
#assert ( -N * ipn == P  ) 
#assert ( -N * 20  == -n ) 
#assert (  P * nt  == -n ) 

k=1;DLB=0
P=4;p=2;q=2
nt=5;

B=20;b=5

ipn=$P;
iter=3

#assert ( B >= p ) 
#assert ( B >= q )



N=1000
#====================================

module load gcc openmpi
#module load intel intelmpi

JobID=${SLURM_JOB_ID}

app=./bin/gcc_dlbsim_debug
params="-P $P -p $p -q $q -M $N $B $b -N $N $B $b -t $nt --ipn $ipn -lambda  10 -K 2 -M 2 -W 2000 --iter-no $iter --timeout 2000 " 
echo "Params: $params"

tempdir=./temp
mkdir -p $tempdir

outfile=$tempdir/tests_${JobID}.out

echo "========================================================================================="
set -x 
rm $outfile
if [ "z${CXX}z" == "zg++z" ] ; then 
	mpirun -n $P --map-by ppr:$ipn:node --output-filename $outfile   $app ${params}
else
	srun  --ntasks-per-node=$ipn -n $P -c $nt -m cyclic:cyclic:* -l --output $outfile $app ${params}
fi
rm *file*.txt *.log pend*.dat comm*.txt
