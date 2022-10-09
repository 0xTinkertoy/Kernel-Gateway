# Kernel-Gateway

## Introduction

This repository contains code of a sample kernel assembled for the gateway device in an emulated automatic watering system.

## Compiler Requirements

A cross-compiler toolchain for AArch32 bare-metal targets (i.e. `arm-none-eabi`) is required to build the kernel.

- GCC 10 ([ARM Embedded Toolchain v10.3-2021.10](https://developer.arm.com/downloads/-/gnu-rm))
- GCC 11 ([ARM Embedded Toolchain v11.3.Rel1](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads))

## Supported Host Systems

- macOS Big Sur
- macOS Monterey
- Ubuntu 20.04 LTS
- Ubuntu 22.04 LTS

Note that other systems are not tested.

## Supported Emulation Platforms

The sample code is designed for Stellaris LM3S811EVB which has a 50 MHz ARM Cortex-M3 processor, 8 KB SRAM and 64 KB Flash.

- QEMU
- [ARM FastModel](https://github.com/0xTinkertoy/ARM-FastModel-LM3S811EVB-Nano)

## Compilation

The assembled kernel uses [CMake](https://cmake.org/) as its build system.  
A `CMakeLists.txt` is provided to build the kernel on macOS and Ubuntu.  
Please refer to the [manual](Documentation/Compilation.md) to build the kernel step by step and run it in QEMU or ARM FastModel.

## IDE Support

The assembled kernel supports CLion.  
Please refer to the [manual](Documentation/IDESetupCLion.md) on the host system and build the kernel in the IDE.

## License

The assembled sample kernel is licensed under BSD-3-Clause.
