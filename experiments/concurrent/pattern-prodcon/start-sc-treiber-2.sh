#!/bin/sh

#experiments for concurrent linearizable treiber stack with prodcon pattern


cd ..; cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-sc-treiber -num_threads=$1 -threshold=$2 -operations=$3 -access_pattern=2 >> ./output/sc-treiber$4 ;
done
