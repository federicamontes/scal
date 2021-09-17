#ifndef CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
#define CLIONPROJECTS_SC_DISTRIBUTED_STACK_H

#include <atomic>
#include <iostream>
#include <vector>
#include <mutex>
#include <time.h>


#include "stack.h"
#include "datastructures/pool.h"
#include "util/threadlocals.h"



namespace sds {

    //thread_local static int index;
    thread_local static uint64_t pop_count;


    template<typename T, typename Stack>
    class SCVectorStack : public Pool<T> {
    public:
        SCVectorStack(uint64_t n, uint64_t k);
        SCVectorStack(uint64_t n, uint64_t k, uint64_t o, uint64_t avgsize);

	    std::vector<Stack *> S; //vector of stacks

        bool put(T element);
        bool get(T *element);


    private:
        uint64_t n; //size
        uint64_t k; //stealing threshold
        uint64_t ops; //number of operations per thread
        uint64_t avg_size;

        int global_index;
        std::vector<int> operations;
        

        //TODO: lock per il vettore di stack
        std::mutex mtx;

        
        T stealing(uint index);
        T stealingRandom(uint index);
        bool push(int index, T element);
        bool pop(int index, T element);
        //int retrieveStack(int index);
        void deleteEmptyStack(std::vector<int> *nonEmptyStack, int index);

    };

    template<typename T, typename Stack>
    SCVectorStack<T, Stack>::SCVectorStack(uint64_t size, uint64_t threshold) {
        
        n = size;
        k = threshold;
        S.resize(n);
        global_index = -1;


    }

    template<typename T, typename Stack>
    SCVectorStack<T, Stack>::SCVectorStack(uint64_t size, uint64_t threshold, uint64_t o, uint64_t avgsize) {

        n = size;
        k = threshold;
        ops = o;
        S.resize(n);
        global_index = -1;
        avg_size = avgsize;

    }
    



    /*template<typename T, typename Stack>
    int SCVectorStack<T, Stack>::retrieveStack(int index) {

       if (global_index == (int) n-1) global_index = -1;
       printf("index passed %d\n", index);
       int last_index = index;
       index = ++global_index;
       printf("retrieveStack %d - %d\n", last_index, index);
       return index;
    }*/





    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::push(int index, T element) {

	    //fprintf(stderr, "SCVectorStack<T, Stack>::TRY push on stack %d of value %lu \n", index, element);
        if (S.at(index)->push(element)) {
            return true;
        }

        return false;
        
    }


    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::pop(int index, T element) {

        if (pop_count >= k) stealing(index);
        bool res = S.at(index)->pop(&element);
        //fprintf(stderr, "SCVectorStack<T, Stack>::TRY pop on stack %d of value %lu \n", index, element);
        if (!res) {
            stealing(index);
            //stealingRandom(index);
        } else {
            pop_count++;
        }
        return res;
    }


    template<typename T, typename Stack>
    T SCVectorStack<T, Stack>::stealing(uint index) {

        T element;
    	//printf("SCVectorStack<T, Stack>::stealing() by thread %u\n", index);
    	bool res;

        uint64_t i = 0;
        
        do {
            if( i == index ) {
                i++;
                continue;
            }
            //printf("i e thread_id: %lu - %u\n", i, index);
            res = S.at(i)->pop(&element);
            if (res) { 
                pop_count = 0;
                return element;
            }
            i++;
        } while( i < n );
  

    	return element;
    }


    template<typename T, typename Stack>
    T SCVectorStack<T, Stack>::stealingRandom(uint index) {
        T element;
        //printf("SCVectorStack<T, Stack>::stealingWithArray()\n");
        bool res;
        /* initialize random seed: */
        srand (time(NULL));

        uint64_t i = rand() % n;
        do {    
            if (i == index) {
                continue;
            }
            //printf("i e thread_id: %lu - %u\n", i, index);
            res = S.at(i)->pop(&element);
            if (res) {
                pop_count = 0;
                return element;
            }
            i = rand() % n;

        } while(i != index);
        
        return element;
    }


    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::put(T element) {
        uint64_t thread_id = scal::ThreadContext::get().thread_id();
        return push(thread_id-1, element);
    }


    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::get(T *element) {
        uint64_t thread_id = scal::ThreadContext::get().thread_id();
        return pop(thread_id-1, *element);
    }


}

#endif //CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
