#!/bin/sh

#experiments for locally linearizable treiber stack with probabilistic pattern


cd ..; cd ..; cd ..; cd out/Debug;

declare -a arr=(1 2 5 10 20 40 60 80)


count=3
for j in "${arr[@]}"; do
	for i in $(seq $count); do
	    ./concurrent-ll-dds-treiber -num_threads=$j -T=$1 -access_pattern=1 -avg_size=$2 >> ./output/ll-dds-treiber$j ;
	done
done
