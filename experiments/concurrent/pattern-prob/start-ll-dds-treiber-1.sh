#!/bin/sh

#experiments for locally linearizable treiber stack with probabilistic pattern


cd ..; cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-ll-dds-treiber -num_threads=$1 -threshold=$2 -T=$3 -access_pattern=1 -avg_size=$4 >> ./output/ll-dds-treiber$5 ;
done
