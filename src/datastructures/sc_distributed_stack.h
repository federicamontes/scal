#ifndef CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
#define CLIONPROJECTS_SC_DISTRIBUTED_STACK_H

#include <atomic>
#include <iostream>
#include <vector>

#include "stack.h"
#include "util/random.h"


namespace sds{

    template<typename T, typename Stack>
    class SCVectorStack {
    public:
        SCVectorStack(uint64_t n, uint32_t k);

	
	    std::vector<Stack> S; //vector of stacks

        bool push(int index, T item);
        bool pop(int index);
	    int allocStack(int *val);


    private:
        uint64_t n; //size
        uint32_t k; //stealing threshold
	    int index;
        
        //TODO: lock per il vettore di stack
        std::mutex mtx;

        T stealing();

    };

    template<typename T, typename Stack>
    SCVectorStack<T, Stack>::SCVectorStack(uint64_t size, uint32_t threshold) {
        n = size;
        k = threshold;
	    index = -1;
        S.resize(n);
    }


    template<typename T, typename Stack>
    int SCVectorStack<T, Stack>::allocStack(int *val) {
	
	   *val = ++index;

	   return index;	
	
    }


    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::push(int index, T item) {
	    printf("SCVectorStack<T, Stack>::push on stack: %d of value: %d \n", index, item);
        return S.at(index).push(item);

    }

    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::pop(int index) {
    	T item;
        bool res = S.at(index).pop(&item);
        printf("SCVectorStack<T, Stack>::pop on stack: %d of value: %d \n", index, item);
        if (!res) { stealing(); }
        return true;
    }


    template<typename T, typename Stack>
    T SCVectorStack<T, Stack>::stealing() {
        T item;
    	printf("SCVectorStack<T, Stack>::stealing()\n");
    	bool res;
            //TODO: lock
            //mtx.lock();
            for (uint64_t i=0; i < this->n; i++) {
                res = S.at(i).pop(&item);
                if (res) break;
            //TODO: unlock
            //mtx.unlock();
            }
    	return item;
    }


}

#endif //CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
