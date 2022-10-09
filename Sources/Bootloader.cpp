//
//  Bootloader.cpp
//  Kernel
//
//  Created by FireWolf on 1/27/21.
//

#include <Debug.hpp>

//
// MARK: - ARM Cortex-M3 Bootloader
//

// C++ initialization routine provided by the compiler
extern "C" void _init(void);

// C++ kernel main routine provided by the assembled kernel
extern "C" void kmain(void);

// Entry point of the bootloader
extern "C"
__attribute__((section(".bootloader.start")))
void start()
{
    // On reset, the bootloader runs in the thread mode
    pinfo("Bootloader started.");

    // Trigger a system call to bring the processor to the handler mode
    asm("svc 0");

    panic("Should never reach at here.");
}

// Bootloader system call handler
__attribute__((section(".bootloader.svchr")))
void BootloaderSVCHandler()
{
    pinfo("Bootloader SVC handler: Will run the kernel in handler mode.");

    extern int gKernelStackTop;

    void* kstack = &gKernelStackTop;

    pinfo("Kernel stack will be set to 0x%p.", kstack);

    // Load the kernel stack pointer
    asm volatile("mov r0, %[kstack] \n"
                 "msr MSP, r0 \n"
                :
                : [kstack] "r" (kstack)
                :
                );

    // The bootloader will trigger a system call to run the kernel in handler mode
    // So that the kernel can exit from the exception and run the first process
    // Assume that the handler knows where the kernel main function is.
    // Normally, we could load the kernel image and jump to the kernel entry point function.
    _init();

    kmain();

    panic("kmain() should never return.");
}

// Address of the bootloader stack
// This symbol is provided and set by the linker script
extern int gBootloaderStack;

// Bootloader interrupt vector table
// This table must be placed at 0x0000
__attribute__((section(".bootloader.isr_table"), used))
volatile void* gBootloaderVectorTable[] =
{
    // The first entry stores the initial stack pointer value
    &gBootloaderStack,
    
    // Exception #1: Reset handler
    // Entry point of the bootloader
    reinterpret_cast<void*>(start),

    // NMI Handler
    nullptr,

    // Hard Fault Handler
    nullptr,

    // Memory Management Handler
    nullptr,

    // Bus Fault Handler
    nullptr,

    // Usage Fault Handler
    nullptr,

    // Reserved
    nullptr,
    nullptr,
    nullptr,
    nullptr,

    // System Call Handler
    reinterpret_cast<void*>(BootloaderSVCHandler),

    // Debug Monitor Handler
    nullptr,

    // Reserved
    nullptr,

    // PendSV Handler
    nullptr,

    // SysTick Handler
    nullptr
};
