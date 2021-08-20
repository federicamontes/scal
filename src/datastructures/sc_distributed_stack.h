#ifndef CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
#define CLIONPROJECTS_SC_DISTRIBUTED_STACK_H

#include <atomic>
#include <iostream>
#include <vector>
#include <mutex>


#include "stack.h"
#include "datastructures/pool.h"

namespace sds {

    thread_local static int index;


    template<typename T, typename Stack>
    class SCVectorStack : public Pool<T> {
    public:
        SCVectorStack(uint64_t n, uint32_t k);

	    std::vector<Stack> S; //vector of stacks

        bool put(T element);
        bool get(T *element);
        //void allocStack(int *val);


    private:
        uint64_t n; //size
        uint32_t k; //stealing threshold
        uint64_t size_collision;
        uint64_t delay;
        int global_index;
        

        //TODO: lock per il vettore di stack
        std::mutex mtx;
        std::mutex mtx_i; //mutex for incrementing index

        
        T stealing();
        bool push(int index, T element);
        bool pop(int index, T element);
        int retrieveStack(void);

    };

    template<typename T, typename Stack>
    SCVectorStack<T, Stack>::SCVectorStack(uint64_t size, uint32_t threshold) {
        
        n = size;
        k = threshold;
        S.resize(n);
        global_index = -1;
    }



    template<typename T, typename Stack>
    int SCVectorStack<T, Stack>::retrieveStack() {

       if (global_index == (int) n-1) global_index = -1;
       index = ++global_index;

       printf("index - global_index %d - %d\n", index, global_index);

       return index;
	
    }



    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::push(int index, T element) {
	    printf("SCVectorStack<T, Stack>::push on stack: %d of value: %lu \n", index, element);
        return S.at(index).push(element);

    }

    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::pop(int index, T element) {
        bool res = S.at(index).pop(&element);
        printf("SCVectorStack<T, Stack>::pop on stack: %d of value: %lu \n", index, element);
        if (!res) { stealing(); }
        return true;
    }


    template<typename T, typename Stack>
    T SCVectorStack<T, Stack>::stealing() {
        T element;
    	printf("SCVectorStack<T, Stack>::stealing()\n");
    	bool res;
            //TODO: lock for sc-stack
            //mtx.lock();
            for (uint64_t i=0; i < n; i++) {
                res = S.at(i).pop(&element);
                if (res) break;
            //TODO: unlock
            //mtx.unlock();
            }
    	return element;
    }



    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::put(T element) {
        index = retrieveStack();
        return push(index, element);
    }


    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::get(T *element) {
        index = retrieveStack();
        return pop(index, *element);
    }


}

#endif //CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
