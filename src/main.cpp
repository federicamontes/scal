
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <mutex> 
#include "datastructures/treiber_stack.h"
//#include "datastructures/elimination_backoff_stack.h"
//#include "datastructures/sc_stack.h"
#include "datastructures/sc_distributed_stack.h"
#include "util/allocation.h"
#include "util/malloc-compat.h"
#include "util/platform.h"
#include "util/scal-time.h"


//TODO: usare il main come seqalt.cc utilizzando una Pool e in ogni std_glue chiamare gli altri stack
//TODO: nel main.cpp chiamo void *ds = ds_new(); all'inizio di tutto prima di creare i thread



uint64_t global_start_time_;
pthread_barrier_t start_barrier_;


void *routine_vector(void *arg) {

	sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>> *vec = (sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>> *) arg;
	//scal::TreiberStack<uint64_t> *stack = (scal::TreiberStack<uint64_t> *) arg;
	//sds::SCVectorStack<uint64_t, scal::EliminationBackoffStack<uint64_t>> *vec = (sds::SCVectorStack<uint64_t, scal::EliminationBackoffStack<uint64_t>> *) arg;

	scal::ThreadLocalAllocator::Get().Init(4096, true);
	std::mutex mtx;
	


	//barrier setup
	int rc = pthread_barrier_wait(&start_barrier_);
  	if (rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    	fprintf(stderr, "%s: pthread_barrier_wait failed.\n", __func__);
    	abort();
 	}

	if (rc == PTHREAD_BARRIER_SERIAL_THREAD) {
	    __sync_bool_compare_and_swap(&global_start_time_, 0, get_utime());
	}


	uint64_t value = 1000;

	for(int i = 0; i < 100; i++) {
		vec->put(value);
		value++;
	}


	for(int i = 0; i < 100; i++) {
		vec->get(&value);
		value--;
	}

	pthread_exit(0);

	return NULL;
}




int main() {

    //scal::TreiberStack<uint64_t> *stack = new scal::TreiberStack<uint64_t>();

	int n = 1;
    sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>> *vec = new sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>>(n, 100);
    
    //eb-stack
    //scal::ThreadLocalAllocator::Get().Init(4096, true);
    //sds::SCVectorStack<uint64_t, scal::EliminationBackoffStack<uint64_t>> *vec = new sds::SCVectorStack<uint64_t, scal::EliminationBackoffStack<uint64_t>>(n, 100);

    //sds::SCVectorStack<int, sc::SCStack<int>> *vec = new sds::SCVectorStack<int, sc::SCStack<int>>(10, 100);


	if (pthread_barrier_init(&start_barrier_, NULL, n)) {
		fprintf(stderr, "%s: error: Unable to init start barrier.\n", __func__);
		abort();
	}

    pthread_t tid[n];

    global_start_time_ = 0;

	for (int i=0; i < n; i++) {
	 	pthread_create(&tid[i], NULL, routine_vector, vec);
	}


    for (int i=0; i < n; i++) {
    	while (pthread_join(tid[i], NULL));
    }

    uint64_t global_end_time_ = get_utime();

    //execution time
    uint64_t exec_time = global_end_time_ - global_start_time_;
    printf("exec_time %lu\n", exec_time);


    return 0;
}


