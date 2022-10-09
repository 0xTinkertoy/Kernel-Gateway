//
//  ThreadScheduler.hpp
//  Kernel-ARM~Gateway
//
//  Created by FireWolf on 3/5/21.
//

#ifndef ThreadScheduler_hpp
#define ThreadScheduler_hpp

#include <Scheduler/Scheduler.hpp>
#include <Execution/Common/KernelServiceRoutines.hpp>
#include "ThreadControlBlock.hpp"
#include "ThreadController.hpp"

struct ThreadScheduler;

namespace Scheduler::Traits
{
    template <>
    struct SchedulerTraits<ThreadScheduler>
    {
        using Task = ThreadControlBlock;
    };
}

struct ThreadScheduler: public Scheduler::Assembler<
        Scheduler::Policies::FIFO::Normal::LinkedListImp<ThreadControlBlock>,
        Scheduler::EventHandlers::TaskCreation::Cooperative::KeepRunningCurrentWithIdleTaskSupport<ThreadScheduler>,
        Scheduler::EventHandlers::TaskBlocked::Common::RunNextWithIdleTaskSupport<ThreadScheduler>,
        Scheduler::EventHandlers::TaskUnblocked::Cooperative::KeepRunningCurrentWithIdleTaskSupport<ThreadScheduler>>
{
public:
    // MARK: Idle Task Support Component IMP

    ///
    /// Get the idle task
    ///
    /// @return The non-null idle task.
    ///
    [[nodiscard]]
    ThreadControlBlock* getIdleTask() const
    {
        return KernelServiceRoutines::GetTaskController<ThreadController>().getThreadAtIndex(0);
    }
};

#endif /* ThreadScheduler_hpp */
