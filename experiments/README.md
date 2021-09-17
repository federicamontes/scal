## How to run the experiments

In the folder experiments/ there are folders referring to the workload -> concurrent/
																       -> prodcon/
																       -> seqalt/

in every folder there are sh scripts for every data structure, both sequentially consistent and linearizable
All output files are redirected in ` scal-local/out/Debug/output  `

## Concurrent

In this folder there is a directory for every benchmark different from the default one -> pattern-prob
																					   -> pattern-seqalt
																					   -> pattern-prodcon

To run scripts in `concurrent/` , `concurrent/pattern-seqalt` and `concurrent/pattern-prodcon` you run 
sh name-of-the-file.sh NUM_THREADS THRESHOLD OPERATIONS NUMBER-OF-FILE

`   sh start-distributed-eb-stack.sh 10 100 1000 1`

the last parameter NUMBER-OF-FILE has to change everytime to save a new file

### Probabilistic pattern

To run scripts in `concurrent/pattern-prob` you run sh name-of-the-file.sh NUM_THREADS THRESHOLD TIME_SECONDS AVERAGE_SIZE NUMBER-OF-FILE

`   sh start-distributed-eb-stack.sh 10 100 5 1000 1`

the last parameter NUMBER-OF-FILE has to change everytime to save a new file


## Prodcon

To run scripts in `prodcon/` you run sh name-of-the-file.sh PRODUCERS CONSUMERS THRESHOLD OPERATIONS NUMBER-OF-FILE

` sh start-distributed-eb-stack.sh 3 2 100 1000 2`

the last parameter NUMBER-OF-FILE has to change everytime to save a new file


## Seqalt

To run scripts in `seqalt/` you run sh name-of-the-file.sh THREADS THRESHOLD ELEMENTS NUMBER-OF-FILE

` sh start-distributed-eb-stack.sh 10 100 1000 10 1`

the last parameter NUMBER-OF-FILE has to change everytime to save a new file





For Concurrent it is possible to configure the type of access pattern using -access_pattern flag taking the values:

* 1: access pattern that probabilistically decides for every operations to be a push or a pop
* 2: access pattern like default prodcon -> always push- always pop /\
* 3: access pattern like seqalt -> it alternates push and pop 
* any other value: access pattern of concurrent that executes push and pop in multiple /\ pattern
