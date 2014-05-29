#include <thread>

#include "GameRunnable.h"
#include "Common.h"
#include "Timer.h"

#define WORLD_SLEEP_CONST 50

#ifdef _WIN32
#include "ServiceWin32.h"
extern int m_ServiceStatus;
#endif

/// Heartbeat for the Game
void GameRunnable::run()
{
    uint64 realCurrTime = 0;
    uint64 realPrevTime = getMSTime();

    uint64 prevSleepTime = 0;// used for balanced full tick time length near WORLD_SLEEP_CONST


    //THIS SHOULD REALLY BE WHILE !SGAME::ISSTOPPED
    while (!m_stop)
    {
        //Update World LoopCounter
        realCurrTime = getMSTime();
        
        uint64 diff = getMSTimeDiff(realPrevTime, realCurrTime);
        
        //Update World TimeDifference
        
        
        realPrevTime = realCurrTime;
        
        if ( diff <= WORLD_SLEEP_CONST + prevSleepTime - diff )
        {
            prevSleepTime = WORLD_SLEEP_CONST + prevSleepTime - diff;
            std::this_thread::sleep_for(std::chrono::milliseconds(prevSleepTime));
        }
        else
        {
            prevSleepTime = 0;
            //Windows Support Here for Services
        }
    }

}