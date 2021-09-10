#include <inttypes.h>

#include <stdio.h>
#include <gflags/gflags.h>
#include <mutex>

#include "benchmark/std_glue/std_pipe_api.h"
#include "datastructures/treiber_stack.h"
#include "datastructures/sc_distributed_stack.h"



//DEFINE_uint64(k, 10, "stealing threshold");


void* ds_new() {

  scal::TreiberStack<uint64_t> *ts[g_num_threads+1];
  sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>> *vstack = new sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>>(
                                              g_num_threads+1, g_threshold, g_operations);
  
  for(uint i=0; i <= g_num_threads; i++) {
    ts[i] = new scal::TreiberStack<uint64_t>();
    vstack->S.at(i) = ts[i];
    //printf("vstack->S.at(i) %p\n", vstack->S.at(i));
  }

  return static_cast<void*>(vstack);
}

char* ds_get_stats(void) {
  return NULL;
}
