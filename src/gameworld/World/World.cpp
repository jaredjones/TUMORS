#include "World.h"

std::atomic<bool> World::m_stopEvent(false);
uint8 World::m_ExitCode = SHUTDOWN_EXIT_CODE;

World::World()
{
    
}

World::~World()
{
    
}