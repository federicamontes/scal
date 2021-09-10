#!/bin/sh

#experiments for concurrent treiber stack
#experiments -> 2 threads (min concurrency) , 10-30-50-70-100-300-500-10000 threads
#experiments -> threshold: depends on #operations #operations/5, #operations/2, #operations/3, threshold=1 (always stealing)

cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./concurrent-sc-treiber -num_threads=$1 -threshold=$2 -operations=$3 >> ./output/output-sc-treiber ;
done

