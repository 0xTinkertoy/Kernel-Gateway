//
//  PL011.hpp
//  Kernel-ARM~ZFastModel
//
//  Created by FireWolf on 10/21/21.
//

#ifndef PL011_hpp
#define PL011_hpp

#include <Types.hpp>
#include <BitOptions.hpp>

#define OSDefineMMIORegister(name, address) \
    static constexpr UInt32 name = address;

namespace PL011
{
    // These addresses are determined by `M3Sample.lisa`
    static constexpr UInt32 kUART0 = 0x4000C000;
    static constexpr UInt32 kUART1 = 0x4000D000;
    static constexpr UInt32 kUART2 = 0x4000E000;

    namespace Registers
    {
        OSDefineMMIORegister(rDATA, 0x000);
        OSDefineMMIORegister(rFLAG, 0x018);
        OSDefineMMIORegister(rLCRH, 0x02C);
        OSDefineMMIORegister(rCTRL, 0x030);
        OSDefineMMIORegister(rIMSC, 0x038);
        OSDefineMMIORegister(rRINS, 0x03C);
        OSDefineMMIORegister(rICR , 0x044);
    }

    static inline UInt16 readRegister16(UInt32 base, UInt32 address)
    {
        return *reinterpret_cast<volatile UInt16*>(base + address);
    }

    static inline void writeRegister16(UInt32 base, UInt32 address, UInt16 value)
    {
        *reinterpret_cast<volatile UInt16*>(base + address) = value;
    }

    static inline bool isSendBusy(UInt32 base)
    {
        return BitOptions(readRegister16(base, Registers::rFLAG)).containsBit(3);
    }

    static inline bool isRecvEmpty(UInt32 base)
    {
        return BitOptions(readRegister16(base, Registers::rFLAG)).containsBit(4);
    }

    static inline void enableUART(UInt32 base)
    {
        UInt16 value = readRegister16(base, Registers::rCTRL) | 0x1;

        writeRegister16(base, Registers::rCTRL, value);
    }

    static inline void disableUART(UInt32 base)
    {
        writeRegister16(base, Registers::rCTRL, 0x0);
    }

    static inline void enableUARTRx(UInt32 base)
    {
        writeRegister16(base, Registers::rCTRL, readRegister16(base, Registers::rCTRL) | (1 << 9));
    }

    static inline void enableUARTTx(UInt32 base)
    {
        writeRegister16(base, Registers::rCTRL, readRegister16(base, Registers::rCTRL) | (1 << 8));
    }

    static inline void disableAllInterrupts(UInt32 base)
    {
        writeRegister16(base, Registers::rIMSC, 0x0);
    }

    static inline void enableRxInterrupt(UInt32 base)
    {
        writeRegister16(base, Registers::rIMSC, readRegister16(base, Registers::rIMSC) | (1 << 4));
    }

    static inline void send(UInt32 base, UInt16 data)
    {
        // Wait until the device is idle
        while (isSendBusy(base));

        writeRegister16(base, Registers::rDATA, data);
    }

    static inline void send(UInt32 base, const void* data, size_t count)
    {
        for (size_t index = 0; index < count; index += 1)
        {
            send(base, reinterpret_cast<const UInt8*>(data)[index]);
        }
    }

    template <typename Object>
    requires (!std::integral<Object>)
    static void send(UInt32 base, const Object& object)
    {
        send(base, &object, sizeof(Object));
    }

    static inline UInt16 receive(UInt32 base)
    {
        // Wait until the device has data available to read
        while (isRecvEmpty(base));

        return readRegister16(base, Registers::rDATA);
    }

    static inline void receive(UInt32 base, void* buffer, size_t count)
    {
        for (size_t index = 0; index < count; index += 1)
        {
            reinterpret_cast<uint8_t*>(buffer)[index] = receive(base);
        }
    }

    template <typename Object>
    requires (!std::integral<Object>)
    static void receive(UInt32 base, Object& object)
    {
        receive(base, &object, sizeof(Object));
    }

    static inline void clearRxInterrupt(UInt32 base)
    {
        writeRegister16(base, Registers::rICR, readRegister16(base, Registers::rICR) | (1 << 4));
    }

    static inline void clearTxInterrupt(UInt32 base)
    {
        writeRegister16(base, Registers::rICR, readRegister16(base, Registers::rICR) | (1 << 5));
    }

    static inline void enableFIFO(UInt32 base)
    {
        writeRegister16(base, Registers::rLCRH, readRegister16(base, Registers::rLCRH) | (1 << 4));
    }

    static inline void disableFIFO(UInt32 base)
    {
        writeRegister16(base, Registers::rLCRH, readRegister16(base, Registers::rLCRH) & ~(1 << 4));
    }

    static inline void init(UInt32 base)
    {
        disableUART(base);

        disableAllInterrupts(base);

        enableUARTRx(base);

        enableUARTTx(base);

        enableUART(base);
    }
}

#endif /* PL011_hpp */
