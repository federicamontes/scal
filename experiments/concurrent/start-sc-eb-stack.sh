#!/bin/sh

#experiments for concurrent linearizable elimination backoff stack with default pattern


cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-sc-eb-stack -num_threads=$1 -threshold=$2 -operations=$3 -access_pattern=4 >> ./output/sc-eb-stack$4 ;
done


