#!/bin/sh

#experiments for concurrent linearizable ts hardware stack with probabilistic pattern


cd ..; cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-sc-ts-hardware-stack -num_threads=$1 -T=$2 -access_pattern=1 -avg_size=$3 >> ./output/sc-ts-hardware-stack$4 ;
done
