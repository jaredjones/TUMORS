#ifndef _H_TUMORS_WORLD
#define _H_TUMORS_WORLD

#include <atomic>

#include "Common.h"

enum ShutdownMask
{
    SHUTDOWN_MASK_RESTART = 1,
    SHUTDOWN_MASK_IDLE    = 2
};

enum ShutdownExitCode
{
    SHUTDOWN_EXIT_CODE = 0,
    ERROR_EXIT_CODE    = 1,
    RESTART_EXIT_CODE  = 2
};

/// The World
class World
{
public:
    static World* instance()
    {
        static World instance;
        return &instance;
    }
    
    /// Are we in the middle of a shutdown?
    static bool IsStopped() { return m_stopEvent; }
private:
    World();
    ~World();
    
    static std::atomic<bool> m_stopEvent;
};

#define sWorld World::instance()

#endif