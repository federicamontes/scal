// Copyright (c) 2012-2013, the Scal Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#define __STDC_FORMAT_MACROS 1  // we want PRIu64 and friends

#include <gflags/gflags.h>
#include <inttypes.h>
#include <string.h>

#include <new>

#include "benchmark/std_glue/std_pipe_api.h"
#include "datastructures/balancer_id.h"
#include "datastructures/distributed_data_structure.h"
#include "datastructures/ms_queue.h"
#include "util/malloc.h"

DEFINE_uint64(p, 80, "number of partial queues");

void* ds_new(void) {
  return static_cast<void*>
      new scal::DistributedDataStructure<uint64_t, scal::MSQueue<uint64_t>, scal::BalancerId>(
          FLAGS_p, 
          g_num_threads + 1,
          new scal::BalancerId());
}

char* ds_get_stats(void) {
  return NULL;
}
