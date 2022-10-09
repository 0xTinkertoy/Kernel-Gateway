//
//  User.cpp
//  Kernel-ARM
//
//  Created by FireWolf on 2/23/21.
//

#include "User.hpp"
#include "Syscall.hpp"
#include "CoAP.hpp"
#include "Memory.h"

// Remove all printing if we are running experiments to measure the stack usage
#ifdef RUN_STACK_EXP
    #define sysprintf (void)
#endif

__attribute__((noreturn))
void idleThread()
{
    while (true)
    {
        sysprintf("Idle thread started.\n");

        asm("wfi");
    }
}

__attribute__((noreturn))
void translator()
{
    uint8_t coap[32];

    char host[16];

    char path[16];

    uint8_t http[64];

    uint16_t port;

    while (true)
    {
        memset(coap, 0, sizeof(coap));

        memset(host, 0, sizeof(host));

        memset(path, 0, sizeof(path));

        port = 80;

        sysprintf("Translator thread: Waiting for a CoAP request...\n");

        int krv = sysRecvData(coap, sizeof(coap));

        if (krv != sizeof(coap))
        {
            sysprintf("Failed to receive the CoAP request message. KVR = %d.\n", krv);

            continue;
        }

        // Examine the header
        uint8_t* ptr = coap;

        struct CoAPHeader* header = reinterpret_cast<struct CoAPHeader*>(ptr);

        if (header->version != 1)
        {
            sysprintf("Header version is invalid.\n");

            continue;
        }

        // Examine options
        ptr += sizeof(struct CoAPHeader) + header->tokenLength;

        size_t delta = 0;

        size_t length;

        while (*ptr != 0xFF)
        {
            // Fetch the option delta and length
            delta += *ptr >> 4;

            length = *ptr & 0xF;

            // Delta/Length = 13/14/15 is not supported
            if (delta == kURIHost)
            {
                memcpy(host, ptr + 1, length);

                sysprintf("Host address is %s.\n", host);
            }
            else if (delta == kURIPort)
            {
                port = *(ptr + 1) << 8 | *(ptr + 2);

                sysprintf("Host port number is %d.\n", port);
            }
            else if (delta == kURIPath)
            {
                memcpy(path, ptr + 1, length);

                sysprintf("Path is %s.\n", path);
            }
            else
            {
                sysprintf("Found an unsupported option.");
            }

            ptr += 1 + length;
        }

        // Examine the payload
        ptr += 1;

        length = HTTPMessageCreate(http, sizeof(http), (enum HTTPMethod) header->code, host, port, path, ptr, 4);

        sysprintf("HTTP request message: %zu bytes.\n", length);

        if (sysSendData(http, length) != length)
        {
            sysprintf("Failed to send the HTTP request message.\n");
        }
        else
        {
            sysprintf("HTTP request message has been sent.\n");
        }
    }
}
