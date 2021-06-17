//
// Created by tripps42 on 17/06/21.
//

#ifndef CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
#define CLIONPROJECTS_SC_DISTRIBUTED_STACK_H

#include <atomic>
#include <iostream>
#include <vector>
#include <pthread.h>

#include "stack.h"
#include "util/random.h"


namespace sds{

    template<typename T>
    class SCVectorStack : public Stack<T> {
    public:
        SCVectorStack(uint64_t n, uint32_t k);
        T* push(T item);
        T* pop();

    private:
        pthread_t tid;
        uint64_t n; //size
        uint32_t k; //stealing threshold
        std::vector<Stack<T>> S; //vector of stacks
        //TODO: lock per il vettore di stack

        T* stealing();
    };

    template<typename T>
    SCVectorStack<T>::SCVectorStack(uint64_t size, uint32_t threshold) {
        n = size;
        k = threshold;
        S.resize(n);
    }

    template<typename T>
    T* SCVectorStack<T>::push(T item) {
        uint64_t index = this->tid % this->n;
        return S.at(index).push(item);
    }

    template<typename T>
    T* SCVectorStack<T>::pop() {
        uint64_t index = this->tid % this->n;
        T* res = S.at(index).pop();
        if (res == NULL) { stealing(); }
        return res;
    }


    template<typename T>
    T* SCVectorStack<T>::stealing() {
        T* res = NULL;
        uint64_t index = this->tid % this->n;
        //TODO: lock
        for (int i=0; i < this->n; i++) {
            res = S.at(i).pop();
            if (res) break;
        //TODO: unlock
        return res;
        }
    }


}

#endif //CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
