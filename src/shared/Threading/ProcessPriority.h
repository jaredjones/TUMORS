//
//  ProcessPriority.h
//  TUMORS
//
//  Created by Jared Jones on 7/20/14.
//
//
#ifndef _TUMORS_PROCESSPRIORITY_H
#define _TUMORS_PROCESSPRIORITY_H

#include "Configuration/Config.h"

#ifdef __linux__
#include <sched.h>
#include <sys/resource.h>
#define PROCESS_HIGH_PRIORITY -15 // [-20, 19], default is 0
#endif

void SetProcessPriority(const std::string logChannel)
{
#if defined(_WIN32) || defined(__linux__)
    ///- Handle affinity for multiple processors and process priority
    uint32 affinity = sConfig.GetIntDefault("UseProcessors", 0);
    bool highPriority = sConfig.GetBoolDefault("ProcessPriority", false);
    
#ifdef _WIN32 // Windows
    
    HANDLE hProcess = GetCurrentProcess();
    if (affinity > 0)
    {
        ULONG_PTR appAff;
        ULONG_PTR sysAff;
        
        if (GetProcessAffinityMask(hProcess, &appAff, &sysAff))
        {
            // remove non accessible processors
            ULONG_PTR currentAffinity = affinity & appAff;
            
            if (!currentAffinity)
                UVO_LOG_ERROR(logChannel, "Processors marked in UseProcessors bitmask (hex) %x are not accessible. Accessible processors bitmask (hex): %x", affinity, appAff);
            else if (SetProcessAffinityMask(hProcess, currentAffinity))
                UVO_LOG_INFO(logChannel, "Using processors (bitmask, hex): %x", currentAffinity);
            else
                UVO_LOG_ERROR(logChannel, "Can't set used processors (hex): %x", currentAffinity);
        }
    }
    
    if (highPriority)
    {
        if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
            UVO_LOG_INFO(logChannel, "Process priority class set to HIGH");
        else
            UVO_LOG_ERROR(logChannel, "Can't set process priority class.");
    }
    
#else // Linux
    
    if (affinity > 0)
    {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        
        for (unsigned int i = 0; i < sizeof(affinity) * 8; ++i)
            if (affinity & (1 << i))
                CPU_SET(i, &mask);
        
        if (sched_setaffinity(0, sizeof(mask), &mask))
            UVO_LOG_ERROR(logChannel, "Can't set used processors (hex): %x, error: %s", affinity, strerror(errno));
        else
        {
            CPU_ZERO(&mask);
            sched_getaffinity(0, sizeof(mask), &mask);
            UVO_LOG_INFO(logChannel, "Using processors (bitmask, hex): %lx", *(__cpu_mask*)(&mask));
        }
    }
    
    if (highPriority)
    {
        if (setpriority(PRIO_PROCESS, 0, PROCESS_HIGH_PRIORITY))
            UVO_LOG_ERROR(logChannel, "Can't set process priority class, error: %s", strerror(errno));
        else
            UVO_LOG_INFO(logChannel, "Process priority class set to %i", getpriority(PRIO_PROCESS, 0));
    }
    
#endif
#endif
}

#endif