//
//  Master.cpp
//  TUMORS
//
//  Created by Jared Jones on 12/7/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#include <vector>
#include <thread>

#include <boost/bind.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "Master.h"
#include "AsyncAcceptor.h"
#include "Common.h"
#include "Util.h"
#include "CommandLine/CliRunnable.h"
#include "Configuration/Config.h"
#include "Cryptography/OpenSSLCrypto.h"
#include "Cryptography/BigNumber.h"
#include "Logging/Log.h"
#include "Threading/ProcessPriority.h"
#include "Timer.h"

#include "World/World.h"
#include "Server/WorldSocket.h"

#ifdef _WIN32
#include "ServiceWin32.h"
char serviceName[] = "TUMORS";
char serviceLongName[] = "TUMORS Environment";
char serviceDescription[] = "The Uvora Multiplayer Online Realm Server.";
/*
 * -1 - not in service mode
 *  0 - stopped
 *  1 - running
 *  2 - paused
 */
int m_ServiceStatus = -1;
#endif

#define WORLD_SLEEP_CONST 50

boost::asio::io_service _ioService;
boost::asio::deadline_timer _freezeCheckTimer(_ioService);
uint32 _worldLoopCounter(0);
uint32 _lastChangeMsTime(0);
uint32 _maxCoreStuckTimeInMs(0);

//Function prototypes
void SignalHandler(const boost::system::error_code &error, int signalNumber);
void ShutdownThreadPool(std::vector<std::thread>& threadPool);
void WorldUpdateLoop();
void FreezeDetectorHandler(const boost::system::error_code& error);

int Master::Run()
{
    OpenSSLCrypto::threadsSetup();
    BigNumber seed1;
    seed1.SetRand(16 * 8);
    
    UVO_LOG_INFO("server.worldserver", "\n<Ctrl-C> to *EMERGENCY* stop.");
    UVO_LOG_INFO("server.worldserver", " ________  __    __  __       __   ______   _______    ______  ");
    UVO_LOG_INFO("server.worldserver", "/        |/  |  /  |/  \\     /  | /      \\ /       \\  /      \\ ");
    UVO_LOG_INFO("server.worldserver", "$$$$$$$$/ $$ |  $$ |$$  \\   /$$ |/$$$$$$  |$$$$$$$  |/$$$$$$  |");
    UVO_LOG_INFO("server.worldserver", "   $$ |   $$ |  $$ |$$$  \\ /$$$ |$$ |  $$ |$$ |__$$ |$$ \\__$$/ ");
    UVO_LOG_INFO("server.worldserver", "   $$ |   $$ |  $$ |$$$$  /$$$$ |$$ |  $$ |$$    $$< $$      \\ ");
    UVO_LOG_INFO("server.worldserver", "   $$ |   $$ |  $$ |$$ $$ $$/$$ |$$ |  $$ |$$$$$$$  | $$$$$$  |");
    UVO_LOG_INFO("server.worldserver", "   $$ |   $$ \\__$$ |$$ |$$$/ $$ |$$ \\__$$ |$$ |  $$ |/  \\__$$ |");
    UVO_LOG_INFO("server.worldserver", "   $$ |   $$    $$/ $$ | $/  $$ |$$    $$/ $$ |  $$ |$$    $$/ ");
    UVO_LOG_INFO("server.worldserver", "   $$/     $$$$$$/  $$/      $$/  $$$$$$/  $$/   $$/  $$$$$$/  ");
    UVO_LOG_INFO("server.worldserver", "                                    http://uvora.com");
    
    
    std::string pidFile = sConfig.GetStringDefault("PID_FILE", "");
    if (!pidFile.empty())
    {
        if (uint32 pid = CreatePIDFile(pidFile))
        {
            UVO_LOG_INFO("server.worldserver", "Daemon PID: %u\n", pid);
        }
        else
        {
            UVO_LOG_ERROR("server.worldserver", "Cannot create PID File %s.\n", pidFile.c_str());
            return 1;
        }
    }
    
    // Set signal handlers (this must be done before starting io_service threads,
    // because otherwise they would unblock and exit)
    boost::asio::signal_set signals(_ioService, SIGINT, SIGTERM);
#if PLATFORM == PLATFORM_WINDOWS
    signals.add(SIGBREAK);
#endif
    signals.async_wait(SignalHandler);
    
    // Star the Boost based thread pool
    int numThreads = sConfig.GetIntDefault("ThreadPool", 1);
    std::vector<std::thread> threadPool;
    
    if (numThreads < 1)
        numThreads = 1;
    
    for (int i = 0; i < numThreads; i++)
        threadPool.push_back(std::thread(boost::bind(&boost::asio::io_service::run, &_ioService)));
    
    // Set process priority according to configuration settings
    SetProcessPriority("server.worldserver");
    
    //Start DB
    
    //Set DB Value to Show OFFLINE
    
    //Initalize World Settings
    sWorld->SetInitialWorldSettings();
    
    // Launch CliRunnable thread
    std::thread* cliThread = nullptr;
#ifdef _WIN32
    if (sConfig.GetBoolDefault("Console.Enable", true) && (m_ServiceStatus == -1)/* need disable console in service mode*/)
#else
    if (sConfig.GetBoolDefault("Console.Enable", true))
#endif
    {
        cliThread = new std::thread(CliThread);
    }
    
    uint16 worldPort = uint16(sWorld->getIntConfig(CONFIG_WORLD_PORT));
    std::string worldListener = sConfig.GetStringDefault("BindIP", "0.0.0.0");
    bool tcpNoDelay = sConfig.GetBoolDefault("Network.TcpNodelay", "true");
    
    AsyncAcceptor<WorldSocket> worldAcceptor(_ioService, worldListener, worldPort, tcpNoDelay);
    
    if (int coreStuckTime = sConfig.GetIntDefault("MaxCoreStuckTime", 0))
    {
        _maxCoreStuckTimeInMs = coreStuckTime * 1000;
        _freezeCheckTimer.expires_from_now(boost::posix_time::seconds(5));
        _freezeCheckTimer.async_wait(FreezeDetectorHandler);
        UVO_LOG_INFO("server.worldserver", "Starting up anti-freeze thread (%u seconds max stuck time)...", coreStuckTime);
    }
    
    UVO_LOG_INFO("server.worldserver", "TUMORS (worldserver) ready...");

    WorldUpdateLoop();
    
    // Shutdown starts here
    ShutdownThreadPool(threadPool);
    
    UVO_LOG_INFO("server.worldserver", "\nHalting process...");
    
    if (cliThread != nullptr)
    {
#ifdef _WIN32
        CancelSynchronousIo(cliThread->native_handle());
#endif
        cliThread->join();
        delete cliThread;
    }
    
    OpenSSLCrypto::threadsCleanup();
    
    return World::GetExitCode();
}

void ShutdownThreadPool(std::vector<std::thread>& threadPool)
{
    _ioService.stop();
    for (auto& thread : threadPool)
        thread.join();
}

void WorldUpdateLoop()
{
    uint64 realCurrTime = 0;
    uint64 realPrevTime = getMSTime();
    
    uint64 prevSleepTime = 0;// used for balanced full tick time length near WORLD_SLEEP_CONST
    
    while (!World::IsStopped())
    {
        ++World::m_worldLoopCounter;
        realCurrTime = getMSTime();
        
        uint64 diff = getMSTimeDiff(realPrevTime, realCurrTime);
        
        //Update World TimeDifference
        //sWorld->Update(diff);
        realPrevTime = realCurrTime;
        
        if ( diff <= WORLD_SLEEP_CONST + prevSleepTime)
        {
            prevSleepTime = WORLD_SLEEP_CONST + prevSleepTime - diff;
            std::this_thread::sleep_for(std::chrono::milliseconds(prevSleepTime));
        }
        else
            prevSleepTime = 0;
#ifdef _WIN32
        if (m_ServiceStatus == 0)
            World::StopNow(SHUTDOWN_EXIT_CODE);
        
        while (m_ServiceStatus == 2)
            Sleep(1000);
#endif
    }
}

void SignalHandler(const boost::system::error_code &error, int signalNumber)
{
    if (!error)
    {
        switch (signalNumber)
        {
            case SIGINT:
            case SIGTERM:
#if PLATFORM == PLATFORM_WINDOWS
            case SIGBREAK:
#endif
                World::StopNow(SHUTDOWN_EXIT_CODE);
            break;
        }
    }
}

void FreezeDetectorHandler(const boost::system::error_code& error)
{
    if (error)
        return;
    
    uint32 curtime = getMSTime();
    uint32 worldLoopCounter = sWorld->m_worldLoopCounter;
    if (_worldLoopCounter != worldLoopCounter)
    {
        _lastChangeMsTime = curtime;
        _worldLoopCounter = worldLoopCounter;
    }
    else if (getMSTimeDiff(_lastChangeMsTime, curtime) > _maxCoreStuckTimeInMs)
    {
        UVO_LOG_ERROR("server.worldserver", "World Thread Hangs, Killing Server!");
        assert(false);
    }
    _freezeCheckTimer.expires_from_now(boost::posix_time::seconds(1));
    _freezeCheckTimer.async_wait(FreezeDetectorHandler);
}

