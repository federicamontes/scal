
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <string>
#include <mutex> 
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <atomic>
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
DEFINE_uint64(avg_size, 80, "number of operations for warm-up phase");
DEFINE_uint64(num_threads, 1, "number of threads");
DEFINE_uint64(threshold, 100, "stealing threshold");
DEFINE_uint64(c, 250, "computational workload");
DEFINE_uint64(access_pattern, 4, "choose which access pattern to execute between default, prodcon, seqalt or probabilistic");
DEFINE_double(prob, 0.5, "probability of enqueue");
DEFINE_double(T, 1, "benchmark time");
DEFINE_bool(print_summary, true, "print execution summary");

using scal::Benchmark;


class ConcurrentBench : public Benchmark {
 public:
  ConcurrentBench(uint64_t num_threads,
               uint64_t thread_prealloc_size,
               void *data)
                   : Benchmark(num_threads,
                               thread_prealloc_size,
                               data) {
        if (pthread_barrier_init(&start_barrier_, NULL, g_num_threads)) {
			fprintf(stderr, "%s: error: Unable to init start barrier.\n", __func__);
			abort();
		}
  }
 protected:
	void bench_func(void);

 private:
 	void insert();
 	void remove();
	pthread_barrier_t start_barrier_;

};


uint64_t g_num_threads;
uint64_t g_threshold;
uint64_t g_operations;
uint64_t g_avg_size;
uint64_t end_benchmark;
uint64_t operations_eff;


void ConcurrentBench::insert(void) {

	Pool<uint64_t> *ds = static_cast<Pool<uint64_t>*>(data_); 

	uint64_t thread_id = scal::ThreadContext::get().thread_id();

	thread_local uint64_t value;

	for(uint64_t i = 0; i < g_operations; i++) {
		value = thread_id * g_operations + i;
		ds->put(value);
		scal::RdtscWait(FLAGS_c);
	}


}


void ConcurrentBench::remove(void) {

	Pool<uint64_t> *ds = static_cast<Pool<uint64_t>*>(data_); 

	thread_local uint64_t value;

	for(uint64_t i = 0; i < g_operations; i++) {
		//res = thread_id * g_operations + j;
		ds->get(&value);
		scal::RdtscWait(FLAGS_c);
		//--j;
	}


}


void ConcurrentBench::bench_func(void) {


	Pool<uint64_t> *ds = static_cast<Pool<uint64_t>*>(data_); 

	uint64_t thread_id = scal::ThreadContext::get().thread_id();

	thread_local uint64_t value;
	thread_local uint64_t operations_thread;

	//printf("bench_func %lu\n", thread_id);


	if (FLAGS_access_pattern == 1) {

		thread_local double p;

		if (thread_id != 1) {
			//warming up the data structure
			for (uint64_t j = 0; j < g_avg_size/g_num_threads; j++) {
				value = thread_id * (g_avg_size/g_num_threads) + j;
				ds->put(value);
			}
		}


		if (thread_id == 1) { 
			sleep(FLAGS_T);
			//printf("sono sveglio %lu\n", thread_id);
			__sync_bool_compare_and_swap(&end_benchmark, 0, 1);
			pthread_exit(0);
		}

		uint64_t i = 0;
		srand (123);
		while(true) { 
			
			p = (double) rand() / RAND_MAX;
			if (p > FLAGS_prob) {
				value = thread_id * g_operations + i;
				ds->put(value);
				operations_thread++;
			} else {
				ds->get(&value);
				operations_thread++;
			}
			if (end_benchmark) {
				//printf("esco %lu\n", thread_id);
				__sync_add_and_fetch(&operations_eff, operations_thread);
				pthread_exit(0);
			}
			i++;
		}

	}else if (FLAGS_access_pattern == 2) {

		insert();

		global_start_time_=0;
		int rc = pthread_barrier_wait(&start_barrier_);
	    if (rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
	      fprintf(stderr, "%s: pthread_barrier_wait failed.\n", __func__);
	      abort();
	    }

	    __sync_bool_compare_and_swap(&global_start_time_, 0, get_utime());
	    remove();


	} else if(FLAGS_access_pattern == 3) {

		for(uint64_t i = 0; i < g_operations; i++) {
			value = thread_id * g_operations + i;
			ds->put(value);
			scal::RdtscWait(FLAGS_c); 
			ds->get(&value);
			scal::RdtscWait(FLAGS_c); 
		}

	} else {


		for(uint64_t i = 0; i < (g_operations*2)/g_num_threads; i++) {
			value = thread_id * g_operations + i;
			ds->put(value);
		}

		//thread_local uint64_t res;
		//thread_local uint64_t j = g_operations-1;
		for(uint64_t i = 0; i < (g_operations*2)/g_num_threads; i++) {
			//res = thread_id * g_operations + j;
			ds->get(&value);
			//--j;
		}

		
	}


}




int main(int argc, const char **argv) {

  	std::string usage("Concurrent micro benchmark.");
  	google::SetUsageMessage(usage);
    google::ParseCommandLineFlags(&argc, const_cast<char***>(&argv), true);


    uint64_t tlsize = scal::Human_size_to_pages(FLAGS_prealloc_size.c_str(),
                                              FLAGS_prealloc_size.size());

    //uint64_t tlsize = 4096;

    g_num_threads = FLAGS_num_threads;
    g_threshold = FLAGS_threshold;
    g_operations = FLAGS_operations;
	g_avg_size = FLAGS_avg_size; //per il warm-up, deve essere almeno g_num_threads


    /*scal::ThreadLocalAllocator::Get().Init(tlsize, true);
	scal::ThreadContext::prepare(g_num_threads+1);
    scal::ThreadContext::assign_context();

    void *ds = ds_new();*/

    ConcurrentBench *benchmark;

    if (FLAGS_access_pattern == 1) {


    	scal::ThreadLocalAllocator::Get().Init(tlsize, true);
		scal::ThreadContext::prepare(g_num_threads+2);
	    scal::ThreadContext::assign_context();

	    void *ds = ds_new();

		benchmark = new ConcurrentBench(
			g_num_threads+1,
			tlsize,
			ds);
		benchmark->run();

	} else {

		scal::ThreadLocalAllocator::Get().Init(tlsize, true);
		scal::ThreadContext::prepare(g_num_threads+2);
	    scal::ThreadContext::assign_context();

	    void *ds = ds_new();

		benchmark = new ConcurrentBench(
			g_num_threads,
			tlsize,
			ds);
		benchmark->run();

	}

	if (FLAGS_print_summary) {

	    uint64_t exec_time;
	    uint64_t num_operations;

	    if (FLAGS_access_pattern == 1) {
	    	exec_time = FLAGS_T*1000000;
	    	num_operations = operations_eff;
	    } else {
	    	exec_time = benchmark->execution_time();
	    	num_operations = ((g_operations*2)/g_num_threads) * g_num_threads * 2;
	    }
	    

		char buffer[1024] = {0};
	    uint32_t n = snprintf(buffer, sizeof(buffer), "{\"threads\": %" PRIu64 " , \"threshold\": %" PRIu64 ", \"runtime\": %" PRIu64 " ,\"operations\": %" PRIu64 " ,\"c\": %" PRIu64 " , \"throughput\": %" PRIu64 ", \"total-operations\": %" PRIu64 "",
	        g_num_threads,
	        g_threshold,
	        exec_time,
	        g_operations,
	        FLAGS_c,
	        (uint64_t)(num_operations / (static_cast<double>(exec_time) / 1000)),
	        num_operations);

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


