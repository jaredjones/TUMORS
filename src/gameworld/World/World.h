#ifndef _H_TUMORS_WORLD
#define _H_TUMORS_WORLD

#include <atomic>

#include "Common.h"
#include "Threading/LockedQueue.h"

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

/// Storage class for commands issued for delayed execution
struct CliCommandHolder
{
    typedef void Print(void*, const char*);
    typedef void CommandFinished(void*, bool success);
    
    void* m_callbackArg;
    char *m_command;
    Print* m_print;
    
    CommandFinished* m_commandFinished;
    
    CliCommandHolder(void* callbackArg, const char *command, Print* zprint, CommandFinished* commandFinished)
    : m_callbackArg(callbackArg), m_command(strdup(command)), m_print(zprint), m_commandFinished(commandFinished)
    {
    }
    
    ~CliCommandHolder() { free(m_command); }
    
private:
    CliCommandHolder(CliCommandHolder const& right) = delete;
    CliCommandHolder& operator=(CliCommandHolder const& right) = delete;
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
    static uint8 GetExitCode() { return m_ExitCode; }
    static bool IsStopped() { return m_stopEvent; }
    static void StopNow(uint8 exitcode) { m_stopEvent = true; m_ExitCode = exitcode; }
    
     void QueueCliCommand(CliCommandHolder* commandHolder) { cliCmdQueue.add(commandHolder); }
    
private:
    World();
    ~World();
    
    static std::atomic<bool> m_stopEvent;
    static uint8 m_ExitCode;
    
    // CLI command holder to be thread safe
    LockedQueue<CliCommandHolder*> cliCmdQueue;
};

#define sWorld World::instance()

#endif