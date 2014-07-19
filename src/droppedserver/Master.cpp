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
#include "Common.h"
#include "Util.h"
#include "Configuration/Config.h"
#include "Cryptography/OpenSSLCrypto.h"
#include "Cryptography/BigNumber.h"
#include "Logging/Log.h"
#include "GameThread/GameRunnable.h"

#include "World/World.h"

boost::asio::io_service _ioService;
boost::asio::deadline_timer _freezeCheckTimer(_ioService);

//Function prototypes
void SignalHandler(const boost::system::error_code &error, int signalNumber);

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
    UVO_LOG_INFO("server.worldserver", "       http://uvora.com");
    
    
    std::string pidFile = sConfig.GetStringDefault("PID_FILE", "");
    if (!pidFile.empty())
    {
        if (uint32 pid = CreatePIDFile(pidFile))
            UVO_LOG_INFO("server.worldserver", "Daemon PID: %u\n", pid);
        else
        {
            UVO_LOG_ERROR("server.worldserver", "Cannot create PID File %s.\n", pidFile.c_str());
            return 1;
        }
    }
    
    // Set signal handlers (this must be done before starting io_service threads,
    // because otherwise they would unblock and exit)
    boost::asio::signal_set signals(_ioService, SIGINT, SIGTERM);
    signals.async_wait(SignalHandler);
    
    // Star the Boost based thread pool
    int numThreads = sConfig.GetIntDefault("ThreadPool", 1);
    std::vector<std::thread> threadPool;
    
    if (numThreads < 1)
        numThreads = 1;
    
    for (int i = 0; i < numThreads; i++)
        threadPool.push_back(std::thread(boost::bind(&boost::asio::io_service::run, &_ioService)));
    
    //Start DB
    
    //Set DB Value to Show OFFLINE
    
    //Initalize Game Settings
    
    //Signal Handlers
    
    GameRunnable *gRunnable = new GameRunnable();
    gRunnable->start();
    gRunnable->join();
    //Make priority highest
    
    //Create CLIRunnable Thread based on Config Options
    
    //Setup processor affinity for linux and windows from config file.
    
    //Create a freeze-thread detector
    
    //Get world port and binding ip from config file
    //Start the Game Socket Manager
    
    //Set DB Value that the game is online
    
    
   /* while(1)
        std::this_thread::sleep_for(std::chrono::nanoseconds(100000000));*/
    
    return 0;
}

void SignalHandler(const boost::system::error_code &error, int signalNumber)
{
    if (!error)
    {
        switch (signalNumber)
        {
            case SIGINT:
            case SIGTERM:
                //ALERT: NEED OT PROGRAM THIS
                //World::StopNow(SHUTDOWN_EXIT_CODE);
                break;
        }
    }
}

