#!/bin/bash

mkdir -p log

dump_files=$(ls execution*.log)

for d in ${dump_files}
do
    dump=./log/${d}.sorted
    sort -k 3 ${d} > ${dump}
    node_extract="${dump}.extract"
    for i in $(seq 0 19)
    do 
	s="\s$i:"
	core_log="${dump}.trace_core_$i"
	core_extract="${core_log}.extract"
       head -n 1 ${dump} > ${core_log}
       grep -B 1 "\#" ${dump} | head -n 1 >>  ${core_log}
	grep $s $dump >> ${core_log}
	python util_extract.py ${core_log} > ${core_extract}
	cat ${core_extract} >> ${node_extract}
    done
    sort -k 5 ${node_extract} > ${node_extract}.sorted
    python util_sum_up.py ${node_extract}.sorted 20.0 > ${node_extract}.sum
    #rm -f $d
done
cat ./log/*.sum >> ./log/all_nodes.extract
sort -k 5 ./log/all_nodes.extract > ./log/all_nodes.sorted
python util_sum_up.py ./log/all_nodes.sorted 2.0 > ./log/all.sum.soreted.sum
