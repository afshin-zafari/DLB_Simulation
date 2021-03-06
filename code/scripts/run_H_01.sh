#!/bin/bash
#SBATCH -A SNIC2017-7-18
#SBATCH -o N01-%j.out
#SBATCH -p node
#SBATCH -t 00:10:00
#SBATCH -N 1
#SBATCH -n 20
#SBATCH -J N01-DTSW

#assert (ipn * nt  == 20 )
#assert ( p  * q   == P  )
#assert ( -N * ipn == P  ) 
#assert ( -N * 20  == -n ) 
#assert (  P * nt  == -n ) 
#assert ( B >= p ) 
#assert ( B >= q )



N=86111; FD=31;
N=155718;FD=50;
N=344444;FD=75;
N=612346;FD=101;

N=42768; FD=17;
N=86111; FD=31;

MTD="galew";
MTD="tc5";

B=1;b=5;
P=1;p=1;q=1;
iter=50;

setup(){
case "$1" in
	1) N=42768;  FD=17;  MTD="galew"; b=5 ;;
	2) N=42768;  FD=17;  MTD="tc5";   b=5 ;;
	3) N=86111;  FD=31;  MTD="galew"; b=5 ;;
	4) N=86111;  FD=31;  MTD="tc5";   b=5 ;;
	5) N=155718; FD=50;  MTD="galew"; b=5 ;;
	6) N=155718; FD=50;  MTD="tc5";   b=5 ;;
	7) N=344444; FD=75;  MTD="galew"; b=5 ;; 
	8) N=344444; FD=75;  MTD="tc5";   b=5 ;;
	9) N=612346; FD=101; MTD="galew"; b=5 ;; 
      10) N=612346; FD=101; MTD="tc5";   b=5 ;;
esac
}
#postfix="debug"
postfix="release"
for S in 1 2 3 4 5 6 7 8 9 10
do
	setup $S
	tempdir="./N${N}_FD${FD}_MTD${MTD}_P${P}_p${p}_q${q}_B${B}_b${b}_iter${iter}"
	mkdir -p $tempdir
	set +x
	source ../main.sh
done
