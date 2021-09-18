#!/bin/sh

#experiments for concurrent linearizable elimination backoff stack with probabilistic pattern


cd ..; cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-sc-eb-stack -num_threads=$1 -T=$2 -access_pattern=1 -avg_size=$3 >> ./output/sc-eb-stack$4 ;
done
