//
//  ThreadDispatcher.hpp
//  Kernel-ARM~Gateway
//
//  Created by FireWolf on 3/5/21.
//

#ifndef ThreadDispatcher_hpp
#define ThreadDispatcher_hpp

#include <Execution/Common/Dispatcher.hpp>
#include <Execution/SimpleThreadBased/KernelServiceRoutines.hpp>
#include "ThreadController.hpp"
#include "ThreadScheduler.hpp"
#include "ThreadSwitcher.hpp"
#include "ThreadExecutionFrameBuilder.hpp"
#include "UART/PL011.hpp"

//
// MARK: - Define kernel service routine functions and the mapper for the dispatcher
//
namespace KernelServiceRoutines
{
    using SyscallUnknownIdentifierRoutine = KernelServiceRoutines::UnknownServiceIdentifier<ThreadControlBlock>;
    OSDefineAndRouteKernelRoutine(kSyscallUnknownIdentifier, ThreadControlBlock, SyscallUnknownIdentifierRoutine)

    static LinkedList<ThreadControlBlock> kTranslators;

    static ThreadControlBlock* kUART1ReceiveInterruptHandler(ThreadControlBlock* current)
    {
        pinfo("UART1 RX Interrupt.");

        // Fetch the waiting thread
        auto waitingThread = const_cast<ThreadControlBlock*>(kTranslators.peekHead());

        [[maybe_unused]] UInt32 tid = GetTaskController<ThreadController>().getThreadIndex(waitingThread);

        if (waitingThread == nullptr)
        {
            pwarning("No translator is available to handle the incoming CoAP message.");

            return current;
        }

        pinfo("Translator %d is available to handle the incoming CoAP message.", tid);

        pinfo("Translator %d requests to receive %zu bytes and has processed %zu bytes.",
                tid, waitingThread->getRequestedSize(), waitingThread->getProcessedSize());

        // Receive data
        while (!waitingThread->isRequestFulfilled())
        {
            if (!PL011::isRecvEmpty(PL011::kUART1))
            {
                waitingThread->serviceRequest(PL011::readRegister16(PL011::kUART1, PL011::Registers::rDATA) & 0xFF);
            }
            else
            {
                // Request is not fully serviced
                // The waiting thread remains blocked
                pinfo("Translator %d's request is partially serviced. # of processed bytes = %zu.",
                        tid, waitingThread->getProcessedSize());

                PL011::clearRxInterrupt(PL011::kUART1);

                return current;
            }
        }

        // The request is fully serviced
        // The waiting thread can be unblocked
        // Reset the processed count
        pinfo("Translator %d's request has been fully serviced. Will be unblocked.", tid);

        waitingThread->endRequest();

        PL011::clearRxInterrupt(PL011::kUART1);

        return GetTaskScheduler<ThreadScheduler>().onTaskUnblocked(current, kTranslators.dequeue());
    }

    static ThreadControlBlock* kRecvDataRoutine(ThreadControlBlock* current)
    {
        current->beginRequest();

        kTranslators.enqueue(current);

        return GetTaskScheduler<ThreadScheduler>().onTaskBlocked(current);
    }

    static ThreadControlBlock* kSendDataRoutine(ThreadControlBlock* current)
    {
        const void* data = current->getSyscallArgument<const void*>();

        auto count = current->getSyscallArgument<size_t>();

        PL011::send(PL011::kUART1, data, count);

        current->setSyscallKernelReturnValue(static_cast<int>(count));

        return current;
    }

#ifndef RUN_STACK_EXP
    static ThreadControlBlock* kPrintRoutine(ThreadControlBlock* current)
    {
        const char* format = current->getSyscallArgument<const char*>();

        auto args = current->getSyscallArgument<va_list*>();

        kvprintf(format, *args);

        return current;
    }
#endif
}

struct ThreadDispatcherRoutineMapper
{
    using Task = ThreadControlBlock;

    using Routine = Task* (*)(Task*);

    using ServiceIdentifier = int;

    Routine operator()(const ServiceIdentifier& identifier)
    {
        using namespace KernelServiceRoutines;

        switch (identifier)
        {
            case 3:
                return kRecvDataRoutine;

            case 4:
                return kSendDataRoutine;

#ifndef RUN_STACK_EXP
            case 5:
                return kPrintRoutine;
#endif

            case 22:
                return &kUART1ReceiveInterruptHandler;

            default:
                kprintf("Unknown identifier = %d\n", identifier);
                return kSyscallUnknownIdentifier;
        }
    }
};

//
// MARK: - Assemble a custom dispatcher for a simple Thread-driven system
//

using ThreadDispatcher = Dispatcher<ThreadControlBlock, int, ThreadDispatcherRoutineMapper, ThreadHandlerSwitcher>;

#endif /* ThreadDispatcher_hpp */
