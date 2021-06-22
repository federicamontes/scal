//
// Created by federicamontes on 17/06/21.
//Implementing a Sequential Consistent stack

#ifndef SC_STACK_H
#define SC_STACK_H

#include <atomic>

#include "datastructures/stack.h"
#include "util/allocation.h"
#include "util/platform.h"
#include "util/atomic_value_new.h"
#include "datastructures/pool.h"


namespace sc {

namespace detail {

template<typename T>
struct Node {
    explicit Node(T item) : next(NULL), data(item) {}

    Node<T>* next;
    T data;
};

} //end detail

template<typename T>
class SCStack : public Stack<T> {

private:
        typedef detail::Node<T> Node;
        typedef TaggedValue<Node*> NodePtr;
        typedef AtomicTaggedValue<Node*, 64, 64> AtomicNodePtr;
        uint32_t size;


        AtomicNodePtr* top_;

public:
    SCStack();

    //virtual detail::Node<T>* push(T* item);
    //virtual detail::Node<T>* pop(void);

    
    bool pop(T *item);
    bool push(T item);


    inline bool empty() { return top_->load().value() == NULL; }

};

template<typename T>
SCStack<T>::SCStack() : top_(new AtomicNodePtr()) {}



template<typename T>
bool SCStack<T>::push(T item) {
    Node* node = new Node(item);
    NodePtr oldTop, newTop;

    printf("SCStack<T>::push %d: \n", item);

    do {
        oldTop = top_->load();
        node->next = oldTop.value();
        newTop = NodePtr(node, oldTop.tag() + 1);
    } while(!top_->swap(oldTop, newTop));

    return true;
}



template<typename T>
bool SCStack<T>::pop(T *item) {
    NodePtr oldTop, newTop;

    do {
        oldTop = top_->load();
        if (oldTop.value() == NULL) return false;
        newTop = NodePtr(oldTop.value()->next, oldTop.tag() + 1);
    } while(!top_->swap(oldTop, newTop));

    *item = oldTop.value()->data;

	printf("SCStack<T>::pop %d: \n", *item);
    return true;
}



} //end sc

#endif //SC_STACK_H
