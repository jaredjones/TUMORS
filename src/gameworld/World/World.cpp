#include "Configuration/Config.h"
#include "Timer.h"
#include "World.h"

std::atomic<bool> World::m_stopEvent(false);
uint8 World::m_ExitCode = SHUTDOWN_EXIT_CODE;
std::atomic<uint32> World::m_worldLoopCounter(0);

World::World()
{
    
}

World::~World()
{
    
}

void World::SetInitialWorldSettings()
{
    //Server startup begin
    uint64 startupBegin = getMSTime();
    
    //TODO: Lookup detour memory management see if we need it
    
    //Initialize the random number generator
    srand((unsigned int)time(NULL));
    
    //Initalize Config Settings
    LoadConfigSettings();
}

void World::LoadConfigSettings(bool reload)
{
    if (reload)
    {
        if (!sConfig.Reload())
        {
            UVO_LOG_ERROR("misc", "World settings reload fail: can't read settings from %s.", sConfig.GetFilename().c_str());
            return;
        }
        sLog.LoadFromConfig();
    }
    
    if (reload)
    {
        uint32 val = sConfig.GetIntDefault("WorldServerPort", 1217);
        if (val != m_int_configs[CONFIG_WORLD_PORT])
        {
            UVO_LOG_ERROR("server.loading", "WorldServerPort option can't be changed at %s reload, using current value (%u).", sConfig.GetFilename().c_str(),
                m_int_configs[CONFIG_WORLD_PORT]);
        }
    }
    else
        m_int_configs[CONFIG_WORLD_PORT] = sConfig.GetIntDefault("WorldServerPort", 1217);
}