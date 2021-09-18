#!/bin/sh

#experiments for concurrent linearizable elimination backoff stack with seqalt pattern


cd ..; cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-sc-eb-stack -num_threads=$1 -threshold=$2 -operations=$3 -access_pattern=3 >> ./output/sc-eb-stack$4 ;
done
