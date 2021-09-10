#include <gflags/gflags.h>

#include "benchmark/std_glue/std_pipe_api.h"
#include "datastructures/ts_timestamp.h"
#include "datastructures/ts_stack_buffer.h"
#include "datastructures/ts_stack.h"
#include "datastructures/sc_distributed_stack.h"


DEFINE_uint64(delay, 0, "delay in the insert operation");


#define TS_DS TSStack<uint64_t, TSStackBuffer<uint64_t, HardwareTimestamp>, HardwareTimestamp>


void* ds_new() {

  TS_DS *ts_[g_num_threads+1];
  sds::SCVectorStack<uint64_t, TS_DS> *vstack = new sds::SCVectorStack<uint64_t, TS_DS>(g_num_threads+1, g_threshold, g_operations);

  for(uint i=0; i <= g_num_threads; i++) {
    ts_[i] = new TS_DS(g_num_threads + 1, FLAGS_delay);
    vstack->S.at(i) = ts_[i];
  }

  return static_cast<void*>(vstack);
}

char* ds_get_stats(void) {
  //return vstack->.ds_get_stats();
	return NULL;
}
