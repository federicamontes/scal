#!/bin/sh

#experiments for concurrent sequential consistent ts hardware stack with probabilistic pattern


cd ..; cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-sc-distributed-ts-hardware-stack -num_threads=$1 -threshold=$2 -T=$3 -access_pattern=1 -avg_size=$4 >> ./output/distributed-ts-hardware-stack$5 ;
done