## How to run the experiments

In the folder experiments/ there are folders referring to the workload -> concurrent/
																       -> prodcon/
																       -> seqalt/

in every folder there are sh scripts for every data structure, both sequentially consistent and linearizable
All output files are redirected in scal-local/out/Debug/output

## Concurrent
To run scripts in concurrent/ you run sh name-of-the-file.sh NUM_THREADS THRESHOLD OPERATIONS ACCESS_PATTERN

`   sh start-distributed-eb-stack.sh 10 100 1000 2`

## Prodcon

To run scripts in prodcon/ you run sh name-of-the-file.sh PRODUCERS CONSUMERS THRESHOLD OPERATIONS

` sh start-distributed-eb-stack.sh 3 2 100 1000 1`


## Seqalt

To run scripts in prodcon/ you run sh name-of-the-file.sh THREADS THRESHOLD ELEMENTS

` sh start-distributed-eb-stack.sh 10 100 1000 10`


For Concurrent it is possible to configure the type of access pattern using -access_pattern flag taking the values:

* 1: access pattern that probabilistically decides for every operations to be a push or a pop
* 2: access pattern like default prodcon -> push-pop /\
* 3: access pattern like seqalt -> it alternates push and pop 
* any other value: access pattern of concurrent that executes push and pop without any particular rule