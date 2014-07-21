//  Copyright (c) 2013 Uvora. All rights reserved.

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <openssl/opensslv.h>
#include <openssl/crypto.h>

#include <boost/program_options.hpp>
#include <boost/version.hpp>

#include "Common.h"
#include "Log.h"
#include "Configuration/Config.h"
#include "Master.h"

using namespace boost::program_options;

#ifndef _TUMORS_CORE_CONFIG
#define _TUMORS_CORE_CONFIG "tumors.cfg"
#endif

#ifndef _TUMORS_BANNER
#define _TUMORS_BANNER "TUMORS :: The Uvora Online Multiplayer Realm Server"
#endif

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

variables_map GetConsoleArguments(int argc, char** argv, std::string& configFile, std::string& configService)
{
    //Silences warning about configService not be used if the OS is not Windows
    (void)configService;
    
    options_description all("Allowed options");
    all.add_options()
    ("help,h", "print usage message")
    ("config,c", value<std::string>(&configFile)->default_value(configFile), "use <arg> as configuration file")
    ;
    
#ifdef _WIN32
    options_description win("Windows platform specific options");
    win.add_options()
    ("service,s", value<std::string>(&configService)->default_value(""), "Windows service options: [install | uninstall]")
    ;
    
    all.add(win);
#endif
    
    variables_map vm;
    try
    {
        store(command_line_parser(argc, argv).options(all).allow_unregistered().run(), vm);
        notify(vm);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }
    
    if (vm.count("help"))
        std::cout << all << "\n";
    
    return vm;
}

//Launch TUMORS
int main(int argc, char** argv)
{
    std::string configFile = _TUMORS_CORE_CONFIG;
    std::string configService;
    
    auto vm = GetConsoleArguments(argc, argv, configFile, configService);
    if (vm.count("help"))
        return 0;
    
#ifdef _WIN32_NOTDONEYET
    if (configService.compare("install") == 0)
        return WinServiceInstall() == true ? 0 : 1;
    if (configService.compare("uninstall") == 0)
        return WinServiceUninstall() == true ? 0 : 1;
    if (configService.compare("run") == 0)
        WinServiceRun();
#endif
    
    if (!sConfig.LoadInitial(configFile.c_str()))
    {
        printf("Invalid or missing configuration file : %s\n", configFile.c_str());
        printf("Verify that the file exists and has \'[TUMORS_CONFIGURATION_FILE]' written in the top of the file!\n");
        printf("NOTE: THIS ERROR ALSO OCCURS IF YOU DO NOT SET THE \"CD\" COMMAND IN TERMINAL TO THE DIRECTORY OF THE PROGRAM FIRST.\n");
        return 1;
    }
    
    UVO_LOG_INFO("server.worldserver", "%s::%s-%s-%s", _TUMORS_BANNER, CONFIG, PLATFORM_TEXT, ARCH);
    UVO_LOG_INFO("server.worldserver", "Using configuration file %s.", configFile.c_str());
    UVO_LOG_INFO("server.worldserver", "Using SSL Version: %s", OPENSSL_VERSION_TEXT);
    int *boostMajor = new int(BOOST_VERSION / 1000000);
    int *boostMinor = new int(BOOST_VERSION / 100 % 1000);
    int *boostSubMinor = new int(BOOST_VERSION % 100);
    UVO_LOG_INFO("server.worldserver", "Using BOOST Version: %i.%i.%i (Lib: %s)", *boostMajor, *boostMinor, *boostSubMinor, BOOST_LIB_VERSION);
    
    delete(boostMajor);
    delete(boostMinor);
    delete(boostSubMinor);
    
    Master *sMaster = new Master();
    int ret = sMaster->Run();
    
    return ret;
}

