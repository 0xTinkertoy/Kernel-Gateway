//
//  Syscall.hpp
//  Kernel-ARM
//
//  Created by FireWolf on 2/5/21.
//

#ifndef Syscall_hpp
#define Syscall_hpp

#include <Execution/SimpleEventDriven/Syscall.hpp>

namespace SyscallIdentifiers
{
    static constexpr int RecvData = 3;
    static constexpr int SendData = 4;
    static constexpr int Print = 5;
}

size_t sysSendData(const void* bytes, size_t count);

size_t sysRecvData(void* bytes, size_t count);

void sysprintf(const char* format, ...);

#endif /* Syscall_hpp */
