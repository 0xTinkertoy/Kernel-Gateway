//
//  Message.hpp
//  Kernel-ARM
//
//  Created by FireWolf on 2/23/21.
//

#ifndef Message_hpp
#define Message_hpp

#include "Types.hpp"

struct Message
{
    enum Type
    {
        /// Internal Type (Moisture Device: User Stack Start Address)
        kMoistureUserStack = 0,

        /// Internal Type (Actuator Device: User Stack Start Address)
        kActuatorUserStack = 1,
        
        /// Internal Type (Gateway  Device: User Thread Stack Start Address)
        kGateWayUserStack = 2,

        /// Internal type (Environment Controller)
        kChangeSoilMoisture = 3,

        /// Internal type (Environment Controller)
        kChangeWaterStatus = 4,

        /// Used by the sensor device (Moisture Sensor)
        kSoilDryAlert = 5,

        /// Used by the sensor device (Moisture Sensor)
        kSoilWetAlert = 6,

        /// Used by the actuator device (Gate Actuator)
        kAckSoilWet = 7,

        /// Used by the actuator device (Water Level Sensor)
        kRunOutOfWaterAlert = 8,
    };
    
    static inline const char* Type2String(Type type)
    {
        switch (type)
        {
            case kMoistureUserStack:
                return "Moisture User Stack";
                
            case kActuatorUserStack:
                return "Actuator User Stack";
                
            case kGateWayUserStack:
                return "Gateway User Stack";
                
            case kChangeSoilMoisture:
                return "Change Soil Moisture";
                
            case kChangeWaterStatus:
                return "Change Water Status";
                
            case kSoilDryAlert:
                return "Soil Dry Alert";
                
            case kSoilWetAlert:
                return "Soil Wet Alert";
                
            case kAckSoilWet:
                return "Ack Soil Wet";
                
            case kRunOutOfWaterAlert:
                return "No Water Alert";
        }
    }

    UInt16 magic;

    UInt16 type;

    UInt32 data;

    Message(Type type, UInt32 data)
    {
        this->magic = 0x4657;

        this->type = type;

        this->data = data;
    }

    static Message moistureUserStack(UInt32 start)
    {
        return {Type::kMoistureUserStack, start};
    }

    static Message actuatorUserStack(UInt32 start)
    {
        return {Type::kActuatorUserStack, start};
    }
    
    static Message gatewayUserStack(UInt32 start)
    {
        return {Type::kGateWayUserStack, start};
    }

    static Message changeSoilMoisture(UInt32 level)
    {
        return {Type::kChangeSoilMoisture, level};
    }

    static Message changeWaterStatus(bool hasWater)
    {
        return {Type::kChangeWaterStatus, hasWater};
    }

    static Message soilDryAlert()
    {
        return {Type::kSoilDryAlert, 0};
    }

    static Message soilWetAlert()
    {
        return {Type::kSoilWetAlert, 0};
    }

    static Message ackSoilWet()
    {
        return {Type::kAckSoilWet, 0};
    }

    static Message runOutOfWaterAlert()
    {
        return {Type::kRunOutOfWaterAlert, 0};
    }
};

#endif /* Message_hpp */
