#!/bin/bash

#experiments for concurrent linearizable elimination backoff stack with default pattern


cd ..; cd ..; cd out/Debug;

declare -a arr=(1 2 5 10 20 40 60 80)


count=3
for j in "${arr[@]}"; do
	for i in $(seq $count); do
	    ./concurrent-sc-eb-stack -num_threads=$j -operations=$1 -access_pattern=4 >> ./output/sc-eb-stack$j ;
	done
done

