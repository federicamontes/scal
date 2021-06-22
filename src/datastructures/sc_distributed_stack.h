#ifndef CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
#define CLIONPROJECTS_SC_DISTRIBUTED_STACK_H

#include <atomic>
#include <iostream>
#include <vector>

#include "stack.h"
#include "util/random.h"


namespace sds{

    template<typename T, typename Stack>
    class SCVectorStack /*: public Stack<T>*/ {
    public:
        SCVectorStack(uint64_t n, uint32_t k);

	int allocStack();

	std::vector<Stack> S; //vector of stacks

        bool push(int index, T item);
        bool pop(int index);


    private:
        uint64_t n; //size
        uint32_t k; //stealing threshold
        
        //TODO: lock per il vettore di stack

        T stealing();

    };

    template<typename T, typename Stack>
    SCVectorStack<T, Stack>::SCVectorStack(uint64_t size, uint32_t threshold) {
        n = size;
        k = threshold;
        S.resize(n);
    }



    int allocStack() {
	
	int index = 0;

	return index;	
	
    }


    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::push(int index, T item) {
	//printf("SCVectorStack<T, Stack>::push %d: \n", item);
        return S.at(index).push(item);

    }

    template<typename T, typename Stack>
    bool SCVectorStack<T, Stack>::pop(int index) {
	T item;
        bool res = S.at(index).pop(&item);
	//printf("SCVectorStack<T, Stack>::pop %d: \n", item);
        if (!res) { stealing(); }
        return true;
    }


    template<typename T, typename Stack>
    T SCVectorStack<T, Stack>::stealing() {
        T item;
	printf("Stealing\n");
	bool res;
        //TODO: lock
        for (int i=0; i < this->n; i++) {
            res = S.at(i).pop(&item);
            if (res) break;
        //TODO: unlock
        return item;
        }
    }


}

#endif //CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
