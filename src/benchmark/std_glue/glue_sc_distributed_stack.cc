#include <inttypes.h>

#include <stdio.h>
#include <gflags/gflags.h>

#include "benchmark/std_glue/std_pipe_api.h"
#include "datastructures/balancer_1random.h"
#include "datastructures/sc_stack.h"
#include "datastructures/treiber_stack.h"
#include "datastructures/sc_distributed_stack.h"
#include "datastructures/distributed_data_structure.h"

DEFINE_uint64(k, 100, "stealing threshold");

DEFINE_bool(hw_random, false, "use hardware random generator instead "
                              "of pseudo");
#define GENERATE_BALANCER() (new scal::Balancer1Random(FLAGS_hw_random))


void* ds_new() {
  sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>, scal::Balancer1Random> *vstack = new sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>, scal::Balancer1Random>(
                                              g_num_threads+1, 100, GENERATE_BALANCER());
  /*int index;
  vstack->allocStack(&index);*/
  return static_cast<void*>(vstack);
}

char* ds_get_stats(void) {
  return NULL;
}