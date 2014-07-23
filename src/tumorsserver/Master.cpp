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
#include "CommandLine/CliRunnable.h"
#include "Configuration/Config.h"
#include "Cryptography/OpenSSLCrypto.h"
#include "Cryptography/BigNumber.h"
#include "Logging/Log.h"
#include "GameThread/GameRunnable.h"
#include "Threading/ProcessPriority.h"

#include "World/World.h"

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
    
    //Initalize Game Settings
    
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
    
    UVO_LOG_INFO("server.worldserver", "Halting process...");
    
    if (cliThread != nullptr)
    {
#ifdef _WIN32
        
        // this only way to terminate CLI thread exist at Win32 (alt. way exist only in Windows Vista API)
        //_exit(1);
        // send keyboard input to safely unblock the CLI thread
        INPUT_RECORD b[4];
        HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
        b[0].EventType = KEY_EVENT;
        b[0].Event.KeyEvent.bKeyDown = TRUE;
        b[0].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[0].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[0].Event.KeyEvent.wRepeatCount = 1;
        
        b[1].EventType = KEY_EVENT;
        b[1].Event.KeyEvent.bKeyDown = FALSE;
        b[1].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[1].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[1].Event.KeyEvent.wRepeatCount = 1;
        
        b[2].EventType = KEY_EVENT;
        b[2].Event.KeyEvent.bKeyDown = TRUE;
        b[2].Event.KeyEvent.dwControlKeyState = 0;
        b[2].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[2].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[2].Event.KeyEvent.wRepeatCount = 1;
        b[2].Event.KeyEvent.wVirtualScanCode = 0x1c;
        
        b[3].EventType = KEY_EVENT;
        b[3].Event.KeyEvent.bKeyDown = FALSE;
        b[3].Event.KeyEvent.dwControlKeyState = 0;
        b[3].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[3].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[3].Event.KeyEvent.wVirtualScanCode = 0x1c;
        b[3].Event.KeyEvent.wRepeatCount = 1;
        DWORD numb;
        WriteConsoleInput(hStdIn, b, 4, &numb);
#endif
        cliThread->join();
        delete cliThread;
    }
    
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
#if PLATFORM == PLATFORM_WINDOWS
            case SIGBREAK:
#endif
                //ALERT: NEED OT PROGRAM THIS
                //World::StopNow(SHUTDOWN_EXIT_CODE);
                break;
        }
    }
}

