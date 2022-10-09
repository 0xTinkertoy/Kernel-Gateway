ENTRY(start)

/* Specific to Stellaris lm3s811evb */
/* Cortex-M3, 8 KB SRAM, 64 KB Flash */
/* https://qemu.readthedocs.io/en/latest/system/arm/stellaris.html */
MEMORY
{
    rom : ORIGIN = 0x00000000, LENGTH = 0x00010000
    ram : ORIGIN = 0x20000000, LENGTH = 0x00002000
}

SECTIONS
{
    . = 0x0;
    .text :
    {
        KEEP(*(.bootloader.isr_table))
        KEEP(*(.bootloader.*))
        *(.text*)
        etext = .;
    } > rom

    .rodata :
    {
        *(.rodata*)
    } > rom

    .preinit_array ALIGN(4):
    {
        __preinit_array_start = .;
        KEEP(*(.preinit_array))
        __preinit_array_end = .;
    } > rom

    .init_array ALIGN(4):
    {
        __init_array_start = .;
        KEEP(*(.init_array))
        __init_array_end = .;
    } > rom

    .fini_array ALIGN(4):
    {
        __fini_array_start = .;
        KEEP(*(.fini_array))
        __fini_array_end = .;
    } > rom

    .kernel.isr_table (NOLOAD) :
    {
        KEEP(*(.kernel.isr_table))
    } > ram

    .data :
    {
        *(.data*)
    } > ram

    .bss :
    {
        *(.bss* COMMON)
    } > ram

    /* Kernel Stack */
    . = ALIGN(8);
    gKernelStackStart = .;
    . = . + 0x400; /* 1 KB of kernel stack memory */
    gKernelStackEnd = .;
    gKernelStackTop = .;
    ebss = .;

    /* Free RAM */
    sram = .;
    eram = 0x20002000;

    /* Bootloader Stack */
    gBootloaderStack = 0x20002000;
}
