#!/bin/sh

#experiments for prodcon distributed elimination backoff stack
#experiments -> 2 threads (min concurrency) , 10-30-50-70-100-300-500-10000 threads
#experiments -> threshold: depends on #operations #operations/5, #operations/2, #operations/3, threshold=1 (always stealing)


cd ..; cd ..; cd out/Debug;

count=10
for i in $(seq $count); do
    ./prodcon-sc-distributed-stack-ebstack -producers=$1 -consumers=$2 -threshold=$3 -operations=$4 >> ./output/prodcon-distributed-eb-stack$5 ;
done


