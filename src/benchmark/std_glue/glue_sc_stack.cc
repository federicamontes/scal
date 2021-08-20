#include <inttypes.h>

#include "benchmark/std_glue/std_pipe_api.h"
#include "datastructures/sc_stack.h"

void* ds_new() {
  sc::SCStack<uint64_t> *sc = new sc::SCStack<uint64_t>();
  return static_cast<void*>(sc);
}

char* ds_get_stats(void) {
  return NULL;
}