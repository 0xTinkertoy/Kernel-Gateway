##
##  CMakeLists.txt
##  Kernel
##
##  Created by FireWolf on 1/22/2021.
##  Revised by FireWolf on 2/18/2022.
##      - Set the compiler toolchain if not invoked by CLion.
##      - Import the Demo/Eval configuration from separate CMake files.
##  Copyright © 2021 FireWolf. All rights reserved.
##

# CMake configurations to build the kernel
cmake_minimum_required(VERSION 3.10)
include(.cmake/Colorful.cmake)

# Set the compiler toolchain if not invoked by CLion
if (DEFINED ENV{CLION_IDE})
    message(STATUS "${BoldYellow}CMakeLists.txt is used by CLion. Will use the toolchain set by CLion.${ColorReset}")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)
    set(HAVE_FLAG_SEARCH_PATHS_FIRST 0)
else()
    message(STATUS "${BoldYellow}CMakeLists.txt is used by CMake. Will use the toolchain set by the user.${ColorReset}")
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/CurrentToolchain.cmake)
endif()

# Project `Name` and `Languages`
project(Kernel-ARM~Gateway@FastModel C CXX ASM)

# Set the kernel executable name
set(TARGET "Kernel")

# Set the kernel executable architecture
set(ARCH "ARM")

# Set the linker script
set(LINKER_SCRIPT "Kernel.ld")

# Append ARM Cortex-M3 specific flags
# If compiler flags and linker flags need to be changed, one must do it before importing the common configuration
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -mthumb -mcpu=cortex-m3 -march=armv7-m")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthumb -mcpu=cortex-m3 -march=armv7-m")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mthumb -mcpu=cortex-m3 -march=armv7-m")

# Add additional definitions
if (DEFINED ENV{KERNEL_DEMO_BUILD})
    message(STATUS "${BoldYellow}Build the kernel for demonstration.${ColorReset}")
    include(CMakeLists.Kernel.Demo.cmake)
elseif(DEFINED ENV{KERNEL_EVAL_BUILD})
    message(STATUS "${BoldYellow}Build the kernel for evaluation.${ColorReset}")
    include(CMakeLists.Kernel.Eval.cmake)
else()
    message(STATUS "${BoldYellow}Build the kernel without any additional definitions.${ColorReset}")
endif()

# Import the common configuration
include(CMakeLists.Kernel.Common.cmake)
