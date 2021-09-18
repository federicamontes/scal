#!/bin/sh

#experiments for locally linearizable treiber stack with prodcon pattern


cd ..; cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-ll-dds-treiber -num_threads=$1 -threshold=$2 -operations=$3 -access_pattern=2 >> ./output/ll-dds-treiber$4 ;
done