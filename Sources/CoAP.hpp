//
//  CoAP.hpp
//  Kernel-ARM~Gateway
//
//  Created by FireWolf on 3/8/21.
//

#ifndef CoAP_hpp
#define CoAP_hpp

#include <stdint.h>
#include <Print.h>
#include "Memory.h"

struct CoAPHeader
{
    uint8_t version: 2;

    uint8_t type: 2;

    uint8_t tokenLength: 4;

    uint8_t code;

    uint16_t msgID;
};

enum CoAPOption
{
    kURIHost = 3,
    kURIPort = 7,
    kURIPath = 11,
};

enum HTTPMethod
{
    kGet = 1,
    kPost = 2,
    kPut = 3
};

const char* HTTPMethod2String(enum HTTPMethod method)
{
    switch (method)
    {
        case kGet:
            return "GET";

        case kPut:
            return "PUT";

        case kPost:
            return "POST";
    }
}

static inline size_t HTTPMessageCreate(void* buffer, size_t count, enum HTTPMethod method, const char* host, size_t port, const char* path, const void* data, size_t length)
{
    size_t offset = snprintf((char*) buffer, count, "%s %s HTTP/1.1\r\nHost: %s:%d\r\n\r\n",
                             HTTPMethod2String(method), path, host, port);

    memcpy((char*) buffer + offset, data, length);

    return offset + length;
}

#endif /* CoAP_hpp */
