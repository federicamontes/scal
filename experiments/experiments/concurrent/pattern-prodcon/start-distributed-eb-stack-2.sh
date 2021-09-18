#!/bin/sh

#script for concurrent sequential consistent elimination backoff stack with prodcon pattern



cd ..; cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-sc-distributed-stack-ebstack -num_threads=$1 -threshold=$2 -operations=$3 -access_pattern=2 >> ./output/distributed-eb-stack$4 ;
done
