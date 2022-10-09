//
//  Syscall.cpp
//  Kernel-ARM
//
//  Created by FireWolf on 2/5/21.
//

#include "Syscall.hpp"
#include <ARM/Syscall.hpp>
#include <cstdarg>

size_t sysSendData(const void* bytes, size_t count)
{
    return syscall(SyscallIdentifiers::SendData, bytes, count);
}

size_t sysRecvData(void* bytes, size_t count)
{
    return syscall(SyscallIdentifiers::RecvData, bytes, count);
}

void sysprintf(const char* format, ...)
{
    va_list args;

    va_start(args, format);

    syscall(SyscallIdentifiers::Print, format, &args);

    va_end(args);
}