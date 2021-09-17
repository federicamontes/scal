#!/bin/sh

#script for concurrent sequential consistent treiber stack with seqalt pattern


cd ..; cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-sc-distributed-stack-treiber -num_threads=$1 -threshold=$2 -operations=$3 -access_pattern=3 >> ./output/distributed-treiber$4 ;
done


