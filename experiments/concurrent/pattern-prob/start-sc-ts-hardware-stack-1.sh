#!/bin/sh

#experiments for concurrent linearizable ts hardware stack with probabilistic pattern


cd ..; cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-sc-ts-hardware-stack -num_threads=$1 -threshold=$2 -T=$3 -access_pattern=1 -avg_size=$4 >> ./output/sc-ts-hardware-stack$5 ;
done
