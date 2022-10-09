//
//  ThreadController.hpp
//  Kernel-ARM~Gateway
//
//  Created by FireWolf on 3/5/21.
//

#ifndef ThreadController_hpp
#define ThreadController_hpp

#include "ThreadControlBlock.hpp"

struct ThreadController
{
private:
    ThreadControlBlock table[4];

public:
    using Task = ThreadControlBlock;

    /// Dumb yet fast allocation algorithm
    ThreadControlBlock* allocate()
    {
        static int index = 0;

        auto block = this->table + index;

        index += 1;

        return block;
    }

    void release(__attribute__((unused)) ThreadControlBlock* block)
    {

    }

    UInt32 getThreadIndex(const ThreadControlBlock* block)
    {
        return block - this->table;
    }

    ThreadControlBlock* getThreadAtIndex(size_t index)
    {
        return this->table + index;
    }
};

#endif /* ThreadController_hpp */
