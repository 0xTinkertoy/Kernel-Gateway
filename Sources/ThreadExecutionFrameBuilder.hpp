//
//  ThreadExecutionFrameBuilder.hpp
//  Kernel-ARM~Gateway
//
//  Created by FireWolf on 3/5/21.
//

#ifndef ThreadExecutionFrameBuilder_hpp
#define ThreadExecutionFrameBuilder_hpp

#include <Execution/Common/KernelServiceRoutines.hpp>
#include <Memory.h>
#include "ThreadControlBlock.hpp"

// TODO: Provided by Execution/SimpleThreadBased

/// Architecture-dependent execution context builder
struct ThreadExecutionContextBuilder_ARM
{
    void operator()(ThreadControlBlock* task, const UInt8* entryPoint)
    {
        pinfo("Setup the context for the thread %d.", KernelServiceRoutines::GetTaskController<ThreadController>().getThreadIndex(task));

        UInt8* sp = task->getStackPointer();

        pinfo("BEFORE: Stack pointer is 0x%p.", sp);

        // Setup the execution context for the trampoline function
        sp -= sizeof(Context);

        auto context = reinterpret_cast<Context*>(sp);

        memset(context, 0xCC, sizeof(Context));

        // Return address
        context->r14 = reinterpret_cast<UInt32>(nullptr);

        // Program counter
        context->r15 = reinterpret_cast<UInt32>(entryPoint);

        // Program status
        // https://developer.arm.com/documentation/dui0552/a/the-cortex-m3-processor/programmers-model/core-registers?lang=en
        context->xpsr = 0x01000000;

        task->setStackPointer(sp);

        pinfo(" AFTER: Stack pointer is 0x%p.", sp);
    }
};

#endif /* ThreadExecutionFrameBuilder_hpp */
