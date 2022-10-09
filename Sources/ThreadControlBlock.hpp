//
//  ThreadControlBlock.hpp
//  Kernel-ARM~Gateway
//
//  Created by FireWolf on 3/5/21.
//

#ifndef ThreadControlBlock_hpp
#define ThreadControlBlock_hpp

#include <Types.hpp>
#include <ARM/Context.hpp>
#include <Scheduler/Scheduler.hpp>
#include <Execution/Common/TaskControlBlockComponents.hpp>

struct IORequest
{
    void* buffer;

    size_t count;

    size_t processed;
};

struct ThreadControlBlock: Scheduler::Schedulable, Listable<ThreadControlBlock>,
        TaskControlBlockComponents::DedicatedNonRecyclableStackSupport<ThreadControlBlock>,
        TaskControlBlockComponents::SystemCallSupport<ThreadControlBlock, Context>
{
private:
    IORequest request;

public:
    // MARK: I/O Request

    void beginRequest()
    {
        this->request.buffer = this->getSyscallArgument<void*>();

        this->request.count = this->getSyscallArgument<size_t>();

        this->request.processed = 0;
    }

    void serviceRequest(UInt8 byte)
    {
        reinterpret_cast<UInt8*>(this->request.buffer)[this->request.processed] = byte;

        this->request.processed += 1;
    }

    void endRequest()
    {
        this->setSyscallKernelReturnValue(this->request.count);
    }

    [[nodiscard]]
    bool isRequestFulfilled() const
    {
        return this->request.processed == this->request.count;
    }

    [[nodiscard]]
    size_t getRequestedSize() const
    {
        return this->request.count;
    }

    [[nodiscard]]
    size_t getProcessedSize() const
    {
        return this->request.processed;
    }
};

#endif /* ThreadControlBlock_hpp */
