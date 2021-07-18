#include <inttypes.h>

#include "benchmark/std_glue/std_pipe_api.h"
#include "datastructures/sc_stack.h"
#include "datastructures/treiber_stack.h"
#include "datastructures/sc_distributed_stack.h"

void* ds_new() {
  sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>> *vstack = new sds::SCVectorStack<uint64_t, scal::TreiberStack<uint64_t>>(5, 100);
  return static_cast<void*>(vstack);
}

char* ds_get_stats(void) {
  return NULL;
}