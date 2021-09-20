#!/bin/sh

#experiments for concurrent linearizable treiber stack with default pattern


cd ..; cd ..; cd out/Debug;

declare -a arr=(1 2 5 10 20 40 60 80)

count=3
for j in "${arr[@]}"; do
	for i in $(seq $count); do
	    ./concurrent-sc-treiber -num_threads=$j -operations=$2 -access_pattern=4 >> ./output/sc-treiber$j ;
	done

done
