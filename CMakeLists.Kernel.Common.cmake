##
##  CMakeLists.txt
##  Kernel
##
##  Created by FireWolf on 1/22/2021.
##  Copyright © 2021 FireWolf. All rights reserved.
##

# Common CMake configurations to build the kernel
# Prerequisite: Define the variable `TARGET`, `ARCH`, `LINKER_SCRIPT`
cmake_minimum_required(VERSION 3.10)

# Set the language standard to `C11` and `C++20`
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

#
# Set the compiler flags
#
# @note The content of `CMAKE_<LANG>_FLAGS_DEBUG` will be appended to `CMAKE_<LANG>_FLAGS` when CMake generates the Makefile for a debug build.
# @
# @note `-ffreestanding` tells the compiler to use freestanding header implementations.
# @note `-fdata-sections -ffunction-sections` tell the compiler to put each function into a separate section,
#        so later the linker can remove all unused symbols and functions.
# @note `--fno-use-cxa-atexit` tells the compiler not to generate tear down code for the kernel.
# @note `-x assembler-with-cpp` tells the assembler that assembly files may contain C directives.
#
set(CMAKE_C_FLAGS                   "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-psabi --specs=nosys.specs -fdata-sections -ffunction-sections -ffreestanding")
set(CMAKE_C_FLAGS_DEBUG             "${CMAKE_C_FLAGS_DEBUG} -O0 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE           "${CMAKE_C_FLAGS_RELEASE} -Os")

set(CMAKE_CXX_FLAGS                 "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-psabi --specs=nosys.specs -fdata-sections -ffunction-sections -ffreestanding -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-use-cxa-atexit")
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEBUG} -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_CXX_FLAGS_RELEASE} -Os")

set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")

message(STATUS "C Compiler Flags: ${CMAKE_C_FLAGS}")
message(STATUS "C Compiler Flags (DEBUG): ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "C Compiler Flags (RELEASE): ${CMAKE_C_FLAGS_RELEASE}")

message(STATUS "C++ Compiler Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "C++ Compiler Flags (DEBUG): ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ Compiler Flags (RELEASE): ${CMAKE_CXX_FLAGS_RELEASE}")

#
# Set the linker flags
#
# @note `-T Kernel.ld` tells the linker to use the custom linker script.
# @note `-nostdlib` tells the linker not to link any standard libraries.
# @note `-nostdlib` also includes `-nostartfiles` that tells the linker
#       not to link any implicit C runtime startup files (e.g. crt0.o).
#       The kernel startup routine serves the same purpose.
# @note `-lgcc` tells the linker to use GCC's internal library that is excluded by the `-nostdlib` option.
# @note `-Wl,--gc-sections` tells the linker to recycle all unused sections.
#
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT} -nostdlib -lgcc -Wl,--gc-sections")

#
# Add additional definitions
#
add_compile_definitions("__KERNEL__")

# Compile the custom C runtime startup files separately
# Used to generate pre-compiled objects
if (EXISTS ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crti.s)
    set(CRTI_SRC_PATH ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crti.s)
elseif(EXISTS ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crti.c)
    set(CRTI_SRC_PATH ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crti.c)
else()
    message(FATAL_ERROR "Cannot find the source of crti.")
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crtn.s)
    set(CRTN_SRC_PATH ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crtn.s)
elseif(EXISTS ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crtn.c)
    set(CRTN_SRC_PATH ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crtn.c)
else()
    message(FATAL_ERROR "Cannot find the source of crtn.")
endif()

message(STATUS "CRTI for ${ARCH}: ${CRTI_SRC_PATH}")
message(STATUS "CRTN for ${ARCH}: ${CRTN_SRC_PATH}")
add_library("CRTI_${ARCH}" OBJECT ${CRTI_SRC_PATH})
add_library("CRTN_${ARCH}" OBJECT ${CRTN_SRC_PATH})

# Generate a list of source files
file(GLOB_RECURSE SOURCE_FILES
        # Common Kernel Code
        Sources/*.cpp
        Sources/*.c
        Sources/*.s

        # Architecture Specific Code
        Dependencies/Architecture/${ARCH}/*.cpp
        Dependencies/Architecture/${ARCH}/*.c
        Dependencies/Architecture/${ARCH}/*.s
        )

# Define the kernel executable
add_executable(${TARGET} ${SOURCE_FILES})

# The kernel relies on pre-compiled startup objects
add_dependencies(${TARGET} CRTI_${ARCH} CRTN_${ARCH})

# Include all Tinkertoy OS modules
add_subdirectory(Dependencies/TinkerLibrary)
add_subdirectory(Dependencies/Scheduler)
add_subdirectory(Dependencies/MemoryAllocator)
add_subdirectory(Dependencies/Execution)
target_link_libraries(${TARGET} PUBLIC TinkerLibrary)
target_link_libraries(${TARGET} PUBLIC Scheduler)
target_link_libraries(${TARGET} PUBLIC MemoryAllocator)
target_link_libraries(${TARGET} PUBLIC Execution)
target_include_directories(${TARGET} PUBLIC Dependencies/Architecture)

# Tell the linker to recompile the kernel if the linker script has been changed
set_target_properties(${TARGET} PROPERTIES LINK_DEPENDS ${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT})

#
# MARK: C Runtime Startup Files (CRT*.o)
#

# Retrieve the path to `crtbegin.o` provided by the compiler
# This command will be executed at the stage of configuring the project.
# One must include the compiler flags when locating these object files.
# The cross compiler may provide different versions based on the machine architecture (e.g. ARM).
message(STATUS "Compiler flags used to locate crtbegin.o: ${CMAKE_C_FLAGS}")
# Note that the variable ${CMAKE_C_FLAGS} is a string and must be transformed to a list,
# otherwise, CMake would pass the space separated flags as a single flag to the compiler.
separate_arguments(CRT_FLAGS NATIVE_COMMAND ${CMAKE_C_FLAGS})
execute_process(
        COMMAND ${CMAKE_C_COMPILER} ${CRT_FLAGS} -print-file-name=crtbegin.o
        OUTPUT_VARIABLE CRT_BEGIN_OBJ_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Compiler supplied crtbegin.o: ${CRT_BEGIN_OBJ_PATH}")

# Retrieve the path to `crtn.o` provided by the compiler
# This command will be executed at the stage of configuring the project
execute_process(
        COMMAND ${CMAKE_C_COMPILER} ${CRT_FLAGS} -print-file-name=crtend.o
        OUTPUT_VARIABLE CRT_END_OBJ_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Compiler supplied crtend.o: ${CRT_END_OBJ_PATH}")

# Copy `crti.o` and `crtn.o` to `Runtime/<arch>/` so that the linker can find them
# This command will be executed before the stage of linking the kernel
add_custom_command(TARGET ${TARGET} PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E echo "Compiled CRTI Object Path: $<TARGET_OBJECTS:CRTI_${ARCH}>")

add_custom_command(TARGET ${TARGET} PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E echo "Compiled CRTN Object Path: $<TARGET_OBJECTS:CRTN_${ARCH}>")

add_custom_command(TARGET ${TARGET} PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_OBJECTS:CRTI_${ARCH}> ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crti.o)

add_custom_command(TARGET ${TARGET} PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_OBJECTS:CRTN_${ARCH}> ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crtn.o)

# Remove `crti.o` and `crtn.o` as well when users clean the build directory
set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crti.o;${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crtn.o")

#
# Rewrite the default C++ linking rule to insert C runtime objects automatically
#
# @ref https://stackoverflow.com/questions/60396645/cmake-link-project-with-c-runtime-object-files-in-correct-order
#
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> \
    <FLAGS> \
    <CMAKE_CXX_LINK_FLAGS> \
    <LINK_FLAGS> \
    ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crti.o \
    ${CRT_BEGIN_OBJ_PATH} \
    <OBJECTS> \
    -o <TARGET> \
    <LINK_LIBRARIES> \
    ${CRT_END_OBJ_PATH} \
    ${CMAKE_SOURCE_DIR}/Dependencies/Runtime/${ARCH}/crtn.o")

#
# MARK: Print the size of the kernel
#

find_program(SIZE "arm-none-eabi-size")
if (NOT SIZE)
    message(WARNING "Cannot find the arm-none-eabi-size executable.")
    message(WARNING "Will not print the size of the kernel.")
else()
    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "The kernel is available at ${CMAKE_BINARY_DIR}/${TARGET}.")
    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${SIZE} ${CMAKE_BINARY_DIR}/${TARGET})
endif()
