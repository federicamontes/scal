#include <inttypes.h>

#include <stdio.h>
#include <gflags/gflags.h>
#include <mutex>

#include "benchmark/std_glue/std_pipe_api.h"
#include "datastructures/elimination_backoff_stack.h"
#include "datastructures/sc_distributed_stack.h"


DEFINE_uint64(k, 100, "stealing threshold");



void* ds_new() {
  sds::SCVectorStack<uint64_t, scal::EliminationBackoffStack<uint64_t>> *vstack = new sds::SCVectorStack<uint64_t, scal::EliminationBackoffStack<uint64_t>>(
                                              g_num_threads+1, 100);
  

  return static_cast<void*>(vstack);
}

char* ds_get_stats(void) {
  return NULL;
}