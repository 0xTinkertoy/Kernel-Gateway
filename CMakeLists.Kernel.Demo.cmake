#
# Add additional definitions for a demo kernel
#
# @note The kernel supports dynamic memory allocations.
#
add_compile_definitions("KERNEL_DYNAMIC_MALLOC_ENABLED")
add_compile_definitions("KERNEL_FORMATTED_PRINT_ENABLED")