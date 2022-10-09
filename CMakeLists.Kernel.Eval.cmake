#
# Add additional definitions for a eval kernel
#
# @note The kernel supports dynamic memory allocations.
#
add_compile_definitions("KERNEL_DYNAMIC_MALLOC_ENABLED")
add_compile_definitions("KERNEL_SIMPLE_PANIC")
add_compile_definitions("RUN_STACK_EXP")