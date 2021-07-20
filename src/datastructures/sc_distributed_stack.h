#ifndef CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
#define CLIONPROJECTS_SC_DISTRIBUTED_STACK_H

#include <atomic>
#include <iostream>
#include <vector>
#include <mutex>

#include "stack.h"
#include "util/random.h"


#include "datastructures/balancer.h"
#include "datastructures/distributed_data_structure_interface.h"
#include "datastructures/pool.h"


namespace sds{

    template<typename T, typename Stack, typename Balancer>
    class SCVectorStack : public Pool<T> {
    public:
        SCVectorStack(uint64_t n, uint32_t k, Balancer* balancer);

	
	    std::vector<Stack> S; //vector of stacks

        bool push(int index, T item);
        bool pop(int index, T item);
	    int allocStack(int *val);

        bool put(T item);
        bool get(T *item);



    private:
        uint64_t n; //size
        uint32_t k; //stealing threshold
	    int index;

        Balancer* balancer_;
        
        //TODO: lock per il vettore di stack
        std::mutex mtx;

        T stealing();

    };

    template<typename T, typename Stack, typename Balancer>
    SCVectorStack<T, Stack, Balancer>::SCVectorStack(uint64_t size, uint32_t threshold, Balancer* balancer) {
        
        n = size;
        k = threshold;
        balancer_ = balancer;
	    index = -1;
        S.resize(n);
    }


    /*template<typename T, typename Stack>
    int SCVectorStack<T, Stack>::allocStack(int *val) {

	   *val = ++index;

	   return index;	
	
    }*/



    template<typename T, typename Stack, typename Balancer>
    bool SCVectorStack<T, Stack, Balancer>::push(int index, T item) {
	    printf("SCVectorStack<T, Stack>::push on stack: %d of value: %lu \n", index, item);
        return S.at(index).push(item);

    }

    template<typename T, typename Stack, typename Balancer>
    bool SCVectorStack<T, Stack, Balancer>::pop(int index, T item) {
    	//T item;
        bool res = S.at(index).pop(&item);
        printf("SCVectorStack<T, Stack>::pop on stack: %d of value: %lu \n", index, item);
        if (!res) { stealing(); }
        return true;
    }


    template<typename T, typename Stack, typename Balancer>
    T SCVectorStack<T, Stack, Balancer>::stealing() {
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



    template<typename T, typename Stack, typename Balancer>
    bool SCVectorStack<T, Stack, Balancer>::put(T item) {
        const uint64_t i = balancer_->put_id();
        return push(i, item);
    }


    template<typename T, typename Stack, typename Balancer>
    bool SCVectorStack<T, Stack, Balancer>::get(T *item) {

        uint64_t i = balancer_->get_id();
        return pop(i, *item);
    }


}

#endif //CLIONPROJECTS_SC_DISTRIBUTED_STACK_H
