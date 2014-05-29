//
//  Master.cpp
//  TUMORS
//
//  Created by Jared Jones on 12/7/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#include "Master.h"
#include "Common.h"
#include "Util.h"
#include "Configuration/Config.h"
#include "Cryptography/OpenSSLCrypto.h"
#include "Cryptography/BigNumber.h"
#include "Logging/Log.h"
#include "GameThread/GameRunnable.h"

#include "World/World.h"

int Master::Run()
{
    OpenSSLCrypto::threadsSetup();
    BigNumber seed1;
    seed1.SetRand(16 * 8);
    
    UVO_LOG_INFO("server.worldserver", "<Ctrl-C> to *EMERGENCY* stop.\n");
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
    
    
    std::string pidFile = sConfig.GetStringDefault("TUMORS_CONFIGURATION_FILE.PID_FILE", "");
    if (!pidFile.empty())
    {
        if (uint32 pid = CreatePIDFile(pidFile))
            UVO_LOG_INFO("server.worldserver", "Daemon PID: %u\n", pid);
        else
        {
            //NEEDS TO BE AN ERRORLOG
            UVO_LOG_ERROR("server.worldserver", "Cannot create PID File %s.\n", pidFile.c_str());
            return 1;
        }
    }
    
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