## Compilation

### Before you start

This page documents details of how to compile the sample kernel via CLI.  
Please refer to [IDE Setup](IDESetupCLion.md) if you wish to use CLion to compile the project.

### Step 1: Clone the repository with all Tinkertoy modules

```bash
git clone https://github.com/0xTinkertoy/Kernel-Moisture.git
cd Kernel-Moisture
git submodule update --init
```

### Step 2: Select a CMake cross-compiler toolchain

This repository provides the following toolchains: 

// TODO: TABLE

If you install a compiler toolchain from other sources,
you may need to update the environment variables in the profile to reflect the actual location of the compiler.

*For demonstration, we will use GCC 11 (11.3.rel1) downloaded from ARM to compile the sample kernel on macOS.*  
- Toolchain: `Toolchains/ARM32_GCC-11_macOS_ARM.cmake`

We now create an alias to selected compiler toolchain.

```bash
ln -s Toolchains/ARM32_GCC-11_macOS_ARM.cmake CurrentToolchain.cmake
```

#### Step 3: Decide the compilation mode

The sample kernel can be built for demonstration or evaluation.  
The kernel built for evaluation will have timer disabled and `kprintf` and serial driver removed.  

To build the kernel for demonstration, please set the environment variable `KERNEL_DEMO_BUILD=1`.

```bash
export KERNEL_DEMO_BUILD=1
```

To build the kernel for evaluation, please set the environment variable `KERNEL_EVAL_BUILD=1`.

```bash
export KERNEL_EVAL_BUILD=1
```

### Step 4: Create the build folder

```bash
mkdir build
```

### Step 5: Run CMake to generate files for the native build system

To generate a DEBUG build:  

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

To generate a RELEASE build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

### Step 6: Compiler the kernel

Please adjust the number of threads `--parallel 8` accordingly.

```bash
cmake --build build --clean-first --parallel 8
```

### Step 7: Run the kernel in QEMU

```bash
qemu-system-arm -cpu cortex-m3 -M lm3s811evb -kernel build/Kernel -serial stdio -serial tcp::10000,server,wait 
```

Note that the emulated board has three serial ports.
The first port is redirected to `stdio` so that you can see the console output in your terminal.  
The second port is redirected to `localhost:10000` and is used by the monitor device to notify the actuator device (e.g, open the water gate).  
