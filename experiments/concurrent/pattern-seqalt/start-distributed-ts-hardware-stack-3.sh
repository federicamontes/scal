#!/bin/sh

#experiments for concurrent sequential consistent ts hardware stack with seqalt pattern


cd ..; cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-sc-distributed-ts-hardware-stack -num_threads=$1 -threshold=$2 -operations=$3 -access_pattern=3 >> ./output/distributed-ts-hardware-stack$4 ;
done
