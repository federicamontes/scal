//
// Created by federicamontes on 17/06/21.
//Implementing a Sequential Consistent stack

#ifndef CLIONPROJECTS_SC_STACK_H
#define CLIONPROJECTS_SC_STACK_H

#include <atomic>

#include "datastructures/stack.h"
#include "util/allocation.h"
#include "util/platform.h"
#include "util/atomic_value_new.h"


namespace sc {
    namespace detail {
        template<typename T>
        struct Node: StackNode {
            explicit Node(T item) : next(NULL), data(item) {}

            Node<T>* next;
            T data;
        };
    } //end detail

template<typename T>
class SCStack : public Stack<T> {

public:
    SCStack();
    SCStack(uint32_t s);
    Node* push(T item);
    Node* pop(void);

    inline bool put(T item) { return push(item); }

    inline State put_state() { return top_->load().tag(); }

    inline bool empty() { return top_->load().value == NULL; }

    inline bool get_return_put_state(T *item, State* put_state);

private:
        typedef sc::detail::Node<T> Node;
        typedef TaggedValue<Node*> NodePtr;
        typedef AtomicTaggedValue<Node*, 64, 64> AtomicNodePtr;
        typedef uint32_t size;

        AtomicNodePtr* top_;
    };

template<typename T>
SCStack<T>::SCStack() : top_(new AtomicNodePtr()) {}

template<typename T>
SCStack<T>::SCStack<typename T>(uint32_t s) {
    size = s;
}


template<typename T>
Node* SCStack<T>::push(T item) {
    Node* node = new Node(item);
    NodePtr oldTop, newTop;
    
    do {
        oldTop = top_.load();
        node->next = oldTop.value();
        newTop = NodePtr(node, oldTop.tag() + 1);
    } while(!top_->swap(oldTop, newTop));

    return node;
}


template<typename T>
Node* SCStack<T>::pop(void) {

}



} //end sc

#endif //CLIONPROJECTS_SC_STACK_H
