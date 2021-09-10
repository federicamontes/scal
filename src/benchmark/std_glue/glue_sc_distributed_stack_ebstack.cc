#include <inttypes.h>

#include <stdio.h>
#include <gflags/gflags.h>
#include <mutex>

#include "benchmark/std_glue/std_pipe_api.h"
#include "datastructures/elimination_backoff_stack.h"
#include "datastructures/sc_distributed_stack.h"


//DEFINE_uint64(k, 100, "stealing threshold");
DEFINE_uint64(collision, 0, "size of the collision array");
DEFINE_uint64(delay, 15000, "time waiting in the collision array");


void* ds_new() {

  uint64_t size_collision = (g_num_threads + 1)/10;
    if (FLAGS_collision != 0) {
      size_collision = FLAGS_collision;
    }
    if (size_collision == 0) { 
      size_collision = 1;
  }
  scal::EliminationBackoffStack<uint64_t> *ebs[g_num_threads+1];

  sds::SCVectorStack<uint64_t, scal::EliminationBackoffStack<uint64_t>> *vstack = new sds::SCVectorStack<uint64_t,
                              scal::EliminationBackoffStack<uint64_t>>(g_num_threads+1, g_threshold, g_operations);


  for(uint i=0; i <= g_num_threads; i++) {
    ebs[i] = new scal::EliminationBackoffStack<uint64_t>(g_num_threads + 1, 
          size_collision, FLAGS_delay);
    vstack->S.at(i) = ebs[i];
  }

  return static_cast<void*>(vstack);
}

char* ds_get_stats(void) {
  return NULL;
}
