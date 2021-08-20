
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <mutex> 
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <gflags/gflags.h>

#include "benchmark/common.h"
#include "benchmark/std_glue/std_pipe_api.h"
#include "datastructures/treiber_stack.h"
//#include "datastructures/elimination_backoff_stack.h"
//#include "datastructures/sc_stack.h"
#include "datastructures/sc_distributed_stack.h"
#include "datastructures/pool.h"

#include "util/allocation.h"
#include "util/malloc.h"
#include "util/malloc-compat.h"
#include "util/platform.h"
#include "util/scal-time.h"
#include "util/random.h"
#include "util/threadlocals.h"
#include "util/workloads.h"

DEFINE_string(prealloc_size, "1g", "tread local space that is initialized");
DEFINE_uint64(num_threads, 1, "number of threads");
DEFINE_bool(barrier, false, "uses a barrier between the enqueues and "
    "dequeues such that first all elements are enqueued, then all elements"
    "are dequeued");


//TODO: usare il main come seqalt.cc utilizzando una Pool e in ogni std_glue chiamare gli altri stack
//TODO: nel main.cpp chiamo void *ds = ds_new(); all'inizio di tutto prima di creare i thread

class ConcurrentBench : public scal::Benchmark {
 public:
  ConcurrentBench(uint64_t num_threads,
               uint64_t thread_prealloc_size,
               void *data)
                   : Benchmark(num_threads,
                               thread_prealloc_size,
                               data) {
  }
 protected:
	void bench_func(void);

 private:

	//pthread_barrier_t start_barrier_;
};


uint64_t g_num_threads;
uint64_t global_start_time_;



void ConcurrentBench::bench_func(void) {

	//sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>> *vec = (sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>> *) arg;
	//scal::TreiberStack<uint64_t> *stack = (scal::TreiberStack<uint64_t> *) arg;
	//sds::SCVectorStack<uint64_t, scal::EliminationBackoffStack<uint64_t>> *vec = (sds::SCVectorStack<uint64_t, scal::EliminationBackoffStack<uint64_t>> *) arg;


	Pool<uint64_t> *ds = static_cast<Pool<uint64_t>*>(data_); 
	//sds::SCVectorStack<uint64_t, Pool<uint64_t>> *ds = static_cast<sds::SCVectorStack<uint64_t, Pool<uint64_t>*>(data_)>;

	/*global_start_time_ = 0;
  int rc = pthread_barrier_wait(&start_barrier_);
  printf("ConcurrentBench::pthread_barrier_wait\n");
  if (rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    fprintf(stderr, "%s: pthread_barrier_wait failed.\n", __func__);
    abort();
  }

	if (rc == PTHREAD_BARRIER_SERIAL_THREAD) {
		printf("ConcurrentBench::global_start_time_ %lu\n", global_start_time_);
    __sync_bool_compare_and_swap(&global_start_time_, 0, get_utime());
	}*/

	uint64_t value = 1000;

	for(int i = 0; i < 100; i++) {
		ds->put(value);
		value++;
	}


	for(int i = 0; i < 100; i++) {
		ds->get(&value);
		value--;
	}

}




int main(int argc, const char **argv) {

  	std::string usage("Concurrent micro benchmark.");
  	google::SetUsageMessage(usage);
    google::ParseCommandLineFlags(&argc, const_cast<char***>(&argv), true);


    uint64_t tlsize = scal::human_size_to_pages(FLAGS_prealloc_size.c_str(),
                                              FLAGS_prealloc_size.size());

    g_num_threads = FLAGS_num_threads;

    scal::ThreadLocalAllocator::Get().Init(tlsize, true);
	  scal::ThreadContext::prepare(g_num_threads+1);
    scal::ThreadContext::assign_context();

    void *ds = ds_new();

	  ConcurrentBench *benchmark = new ConcurrentBench(
		  g_num_threads,
		  tlsize,
		  ds);
	  benchmark->run();

   

    return 0;
}


