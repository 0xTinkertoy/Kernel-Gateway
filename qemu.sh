qemu-system-arm -cpu cortex-m3 -M lm3s811evb -kernel build/Kernel -serial stdio -serial tcp::30001,server,wait -S -gdb tcp::1234
