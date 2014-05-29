//
//  Timer.h
//  TUMORS
//
//  Created by Jared Jones on 12/26/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#ifndef TUMORS_TIMER_h
#define TUMORS_TIMER_h

#include <chrono>

#include "Common.h"


inline uint64 getMSTime()
{
    using namespace std::chrono;
    
    static const uint64 startTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    
    uint64 presentTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    
    return presentTime - startTime;
    
}

inline uint64 getMSTimeDiff(uint64 oldMSTime, uint64 newMSTime)
{
    // getMSTime() have limited data range and this is case when it overflow in this tick
    // However this is very unlikely with a 64bit unsigned int
    if (oldMSTime > newMSTime)
        return (0xFFFFFFFFFFFFFFFF - oldMSTime) + newMSTime;
    else
        return newMSTime - oldMSTime;
}

inline uint64 GetMSTimeDiffToNow(uint64 oldMSTime)
{
    return getMSTimeDiff(oldMSTime, getMSTime());
}
#endif
