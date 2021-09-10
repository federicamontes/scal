
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
DEFINE_uint64(operations, 80, "number of operations per thread");
DEFINE_uint64(num_threads, 1, "number of threads");
DEFINE_uint64(threshold, 100, "stealing threshold");
DEFINE_uint64(c, 5000, "computational workload");
DEFINE_bool(barrier, false, "uses a barrier between the enqueues and "
    "dequeues such that first all elements are enqueued, then all elements"
    "are dequeued");
DEFINE_bool(print_summary, true, "print execution summary");



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
uint64_t g_threshold;
uint64_t g_operations;
uint64_t global_start_time_;






void ConcurrentBench::bench_func(void) {


	Pool<uint64_t> *ds = static_cast<Pool<uint64_t>*>(data_); 

	uint64_t thread_id = scal::ThreadContext::get().thread_id();

	thread_local uint64_t value;


	for(uint64_t i = 0; i < g_operations; i++) {
		value = thread_id * g_operations + i;
		ds->put(value);
	}

	//thread_local uint64_t res;
	//thread_local uint64_t j = g_operations-1;
	for(uint64_t i = 0; i < g_operations; i++) {
		//res = thread_id * g_operations + j;
		ds->get(&value);
		//--j;
	}

}




int main(int argc, const char **argv) {

  	std::string usage("Concurrent micro benchmark.");
  	google::SetUsageMessage(usage);
    google::ParseCommandLineFlags(&argc, const_cast<char***>(&argv), true);


    /*uint64_t tlsize = scal::Human_size_to_pages(FLAGS_prealloc_size.c_str(),
                                              FLAGS_prealloc_size.size());*/

    uint64_t tlsize = 4096;

    g_num_threads = FLAGS_num_threads;
    g_threshold = FLAGS_threshold;
    g_operations = FLAGS_operations;



    scal::ThreadLocalAllocator::Get().Init(tlsize, true);
	scal::ThreadContext::prepare(g_num_threads+1);
    scal::ThreadContext::assign_context();

    void *ds = ds_new();

	ConcurrentBench *benchmark = new ConcurrentBench(
		g_num_threads,
		tlsize,
		ds);
	benchmark->run();


	if (FLAGS_print_summary) {

	    uint64_t exec_time = benchmark->execution_time();

	    uint64_t num_operations = g_operations * g_num_threads;


		char buffer[1024] = {0};
	    uint32_t n = snprintf(buffer, sizeof(buffer), "{\"threads\": %" PRIu64 " , \"threshold\": %" PRIu64 ", \"runtime\": %" PRIu64 " ,\"operations\": %" PRIu64 " ,\"c\": %" PRIu64 " ,\"throughput\": %" PRIu64 "",
	        g_num_threads,
	        g_threshold,
	        exec_time,
	        g_operations,
	        FLAGS_c,
	        (uint64_t)(num_operations / (static_cast<double>(exec_time) / 1000)));

	    if (n != strlen(buffer)) {
	      fprintf(stderr, "%s: error: failed to create summary string\n", __func__);
	      abort();
	    }

	    char *ds_stats = ds_get_stats();
	    if (ds_stats != NULL) {
	      if (n + strlen(ds_stats) >= 1023) {  // separating space + '\0'
	        fprintf(stderr, "%s: error: strings too long\n", __func__);
	        abort();
	      }
	      strcat(buffer, " ");
	      strcat(buffer, ds_stats);
	      strcat(buffer, "}");
	    }
	    printf("%s\n", buffer);

	}
	

    return EXIT_SUCCESS;
}


