//
//  Main.cpp
//  Kernel
//
//  Created by FireWolf on 1/27/21.
//

#include <Debug.hpp>
#include <ARM/v7-M/InterruptVectorTable.hpp>
#include <Execution/SimpleThreadBased/KernelServiceRoutines.hpp>
#include <MemoryAllocator/FreeListAllocator.hpp>
#include "ThreadController.hpp"
#include "ThreadDispatcher.hpp"
#include "ThreadScheduler.hpp"
#include "CMSIS/ARMCM3.h"
#include "UART/PL011.hpp"
#include "Message.hpp"
#include "User.hpp"

//
// Deployment: Thread scheduler
//
OSDeclareTaskSchedulerWithKernelServiceRoutine(ThreadScheduler, scheduler);

//
// Deployment: Thread Controller
//
OSDeclareTaskControllerWithKernelServiceRoutine(ThreadController, controller);

//
// Deployment: Kernel Memory Allocator
//
OSDeclareMemoryAllocatorWithKernelServiceRoutines(FreeListAllocator<ConstantAligner<8>>, kMemoryAllocator);

//
// Deployment: Startup Routines
//
static void initMemoryManager()
{
    pinfo("Initializing the kernel memory allocator...");

    extern UInt8 sram, eram;

    pinfo("Free memory starts at 0x%p and ends at 0x%x (%d bytes).", &sram, &eram, &eram - &sram);

    passert(kMemoryAllocator.init(&sram, &eram - &sram), "Failed to configure the kernel memory allocator.");
}

static void initInterruptTable()
{
    pinfo("Initializing the kernel interrupt vector table...");

    InterruptVectorTable::setup();

    InterruptVectorTable::registerHandler(11, InterruptVectorTable::AssemblyHandler(KernelEntryPoint));
}

static void initUART1()
{
    pinfo("Configuring UART1...");

    // Both QEMU and Fast Model have UART0,1,2 enabled by default.
    // It is possible to disable all UART ports when the Fast Model platform starts.
    // If these ports are disabled, we need to initialize them manually as follows.
    // ```
    //    PL011::disableUART(PL011::kUART1);
    //    {
    //        PL011::disableAllInterrupts(PL011::kUART1);
    //        PL011::enableUARTRx(PL011::kUART1);
    //        PL011::enableUARTTx(PL011::kUART1);
    //        PL011::enableFIFO(PL011::kUART1);
    //        PL011::enableRxInterrupt(PL011::kUART1);
    //    }
    //    PL011::enableUART(PL011::kUART1);
    // ```

    // Turn on the RX interrupt on UART1
    // IRQ number is 22 (See LM3S811 Manual)
    PL011::enableRxInterrupt(PL011::kUART1);

    NVIC_EnableIRQ(Interrupt6_IRQn);

    NVIC_SetPriority(Interrupt6_IRQn, 255);

    InterruptVectorTable::registerHandler(22, InterruptVectorTable::AssemblyHandler(KernelEntryPoint));

    // By default, the hardware generates an interrupt once a byte is received
    // (i.e. The FIFO buffer can only hold a single byte)
    // We enable the FIFO buffer for both TX and RX channels
    // so that the device generates an interrupt once 8 bytes are received
    // FIFO buffers are 16 bytes long
    // We could also configure the watermark to choose a different level instead, such as 4 bytes.
    // Reference: Section 11.3.4 FIFO Operation in LM3S811 Manual

    // Enable the FIFO operations
    PL011::enableFIFO(PL011::kUART1);
}

static void initThreads()
{
    pinfo("========================================================================================================");

    pinfo("Preconfiguring threads...");

    static constexpr size_t kDefaultStackSize = 1024;

    using namespace KernelServiceRoutines::CreateThread;

    using ThreadInitializer = KPI::TaskInitializerBuilderWithArgs<
            ThreadControlBlock,
            KPI::AllocateDedicatedStack<ThreadControlBlock>,
            KPI::SetupExecutionContext<ThreadControlBlock, ThreadExecutionContextBuilder_ARM>>;
    
    // Create the idle thread
    passert(ThreadInitializer{}(controller.allocate(), kDefaultStackSize, reinterpret_cast<UInt8*>(idleThread)),
            "Failed to initialize the idle thread.");

    pinfo("========================================================================================================");

    // Create three translator threads
    for (int index = 0; index < 3; index += 1)
    {
        auto thread = controller.allocate();

        ThreadInitializer{}(thread, kDefaultStackSize, reinterpret_cast<UInt8*>(translator));

        scheduler.ready(thread);

        pinfo("========================================================================================================");
    }

#ifdef RUN_STACK_EXP
    for (int index = 0; index < 4; index += 1)
    {
        PL011::send(PL011::kUART1, Message::gatewayUserStack(reinterpret_cast<UInt32>(controller.getThreadAtIndex(index)->getStackPointer() + sizeof(Context) - kDefaultStackSize)));
    }
#endif
}

//
// Deployment: Kernel Main Routine
//
extern "C" void kmain(void)
{
    pinfo("Kernel main function started.");

    kprintf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    kprintf("Hello, Tinkertoy~Kernel-ARM~Gateway! \n");
    kprintf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    // Configure the kernel memory allocator
    initMemoryManager();

    // Configure the kernel interrupt table
    initInterruptTable();

    // Configure UART1 and RX interrupts
    initUART1();

    // Configure threads
    initThreads();

    // Dispatcher
    // We assume that the idle handler was running before we first enter the dispatcher
    pinfo("Enter the dispatcher.");

    ThreadDispatcher dispatcher(controller.getThreadAtIndex(0), scheduler.next());

    dispatcher.dispatch();
}
