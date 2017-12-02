#!/bin/bash
#SBATCH -A SNIC2017-1-448
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

DLB=0
P=2;p=$P;q=1
nt=$[ 20 / $P ];
# 1MB   2M    5M   10M   20M  50M   100M
x=353; #500; 790; 1118; 1581; 2500;  3535
M=$[ $x * $x * 8]; 
K=$[ $P / 2 ] 
LAMBDA_BAR=5
LAMBDA_STAR=$[ $P / $K * ${LAMBDA_BAR} ]
DLB_PARS="--dlb --silent-dur 10 --dlb-threshold ${LAMBDA_BAR} --failure-max 5 -- silent-mode 1 "
#DLB_PARS=""

B=$[ ${LAMBDA_STAR} * $K]
N=$[ $x * $B ]
b=5

ipn=$P;
iter=3

#assert ( B >= p ) 
#assert ( B >= q )




#====================================

module load gcc openmpi
#module load intel intelmpi

JobID=${SLURM_JOB_ID}

app=./bin/gcc_dlbsim_debug
params="-P $P -p $p -q $q -M $N $B $b -N $N $B $b -t $nt --ipn $ipn -lambda ${LAMBDA_BAR} -K $K -W 1000 --iter-no $iter --timeout 100 ${DLB_PARS}" 
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
