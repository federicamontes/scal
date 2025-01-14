// Copyright (c) 2012-2015, the Scal Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

// Implementing the non-blocking lock-free stack from:
//
// R. K. Treiber. Systems Programming: Coping with Parallelism. RJ 5118, IBM
// Almaden Research Center, April 1986.

#ifndef SCAL_DATASTRUCTURES_TREIBER_STACK_H_
#define SCAL_DATASTRUCTURES_TREIBER_STACK_H_

#include "datastructures/distributed_data_structure_interface.h"
#include "datastructures/stack.h"
#include "util/allocation.h"
#include "util/atomic_value_new.h"
#include "util/platform.h"
#include "util/threadlocals.h"

namespace scal {

namespace detail {

template<typename T>
struct Node : ThreadLocalMemory<64> {
  explicit Node(T item) : next(NULL), data(item) { }

  Node<T>* next;
  T data;
};

}  // namespace detail


template<typename T>
class TreiberStack : public Stack<T> {
 public:
  TreiberStack();
  bool push(T item);
  bool pop(T *item);

  bool try_push(T item, uint64_t top_old_tag);
  uint8_t try_pop(T *item, uint64_t top_old_tag, State* put_state);

  // Satisfy the DistributedQueueInterface

  inline bool put(T item) {
    return push(item);
  }

  inline State put_state() {
    return top_->load().tag();
  }

  inline bool empty() {
    return top_->load().value() == NULL;
  }

  inline bool get_return_put_state(T *item, State* put_state);


  inline void printStack(int index) {
    Node* n = new Node(0);
    n = top_->load().value();
    while(n->next != NULL) {
      printf("Stampo treiber-stack di %d con %lu\n", index, n->data);
      n = n->next;
    }
  }

 private:
  typedef detail::Node<T> Node;
  typedef TaggedValue<Node*> NodePtr;
  typedef AtomicTaggedValue<Node*, 64, 64> AtomicNodePtr;

  AtomicNodePtr* top_;
};


template<typename T>
TreiberStack<T>::TreiberStack() : top_(new AtomicNodePtr()) {
}




template<typename T>
bool TreiberStack<T>::push(T item) {
  Node* n = new Node(item);
  NodePtr top_old;
  NodePtr top_new;

  //printf("TreiberStack<T>::push %lu: \n", item);
  do {
    top_old = top_->load();
    n->next = top_old.value();
    top_new = NodePtr(n, top_old.tag() + 1);
  } while (!top_->swap(top_old, top_new));
  return true;
}


template<typename T>
bool TreiberStack<T>::pop(T *item) {
  NodePtr top_old;
  NodePtr top_new;
  do {
    top_old = top_->load();
    if (top_old.value() == NULL) {
      return false;
    }
    top_new = NodePtr(top_old.value()->next, top_old.tag() + 1);
  } while (!top_->swap(top_old, top_new));
  *item = top_old.value()->data;
  //printf("TreiberStack<T>::pop %lu: \n", *item);
  return true;
}



// Tailored for the LRU distributed stack implementation.
// Using only 8 bit of the top pointers tag as ABA count 
// for put operations.
template<typename T>
bool TreiberStack<T>::try_push(T item, uint64_t top_old_tag) {
  Node* n = new Node(item);
  NodePtr top_old;
  NodePtr top_new;
  top_old = top_->load();

  uint16_t tag_old = top_old.tag();
  uint16_t put_tag = tag_old >> 8;
  uint16_t get_tag = tag_old & 0x00FF;

  if (top_old_tag == put_tag) {
    n->next = top_old.value();

    put_tag += 1;
    if (put_tag == 256) {
      put_tag = 0;
    }

    top_new = NodePtr(n, (put_tag << 8) ^ get_tag);
    if (top_->swap(top_old, top_new)) {
      return true;
    }
  }
  return false;
}

// Tailored for the LRU distributed stack implementation.
// Using only 8 bit of the top pointers tag as ABA count 
// for get operations.
template<typename T>
uint8_t TreiberStack<T>::try_pop(
    T *item, uint64_t top_old_tag, State* put_state) {
  NodePtr top_old;
  NodePtr top_new;
  top_old = top_->load();

  uint16_t tag_old = top_old.tag();
  uint16_t put_tag = tag_old >> 8;
  uint16_t get_tag = tag_old & 0x00FF;

  if (top_old_tag == get_tag) {
    if (top_old.value() == NULL) {
      *put_state = top_old.tag();
      return 1;
    }

    get_tag += 1;
    if (get_tag == 256) {
      get_tag = 0;
    }

    top_new = NodePtr(top_old.value()->next, (put_tag << 8) ^ get_tag);
    if (top_->swap(top_old, top_new)) {
      *item = top_old.value()->data;
      return 0;
    }
  }
  return 2;
}

template<typename T>
bool TreiberStack<T>::get_return_put_state(T* item, State* put_state) {
  NodePtr top_old;
  NodePtr top_new;
  do {
    top_old = top_->load();
    if (top_old.value() == NULL) {
      *put_state = top_old.tag();
      return false;
    }
    top_new = NodePtr(top_old.value()->next, top_old.tag() + 1);
  } while (!top_->swap(top_old, top_new));
  *item = top_old.value()->data;
  *put_state = top_old.tag();
  return true;
}


}  // namespace scal

#endif  // SCAL_DATASTRUCTURES_TREIBER_STACK_H_
