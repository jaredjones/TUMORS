//  Copyright (c) 2013 Uvora. All rights reserved.

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <openssl/opensslv.h>
#include <openssl/crypto.h>

#include <boost/version.hpp>
#include "Common.h"
#include "Log.h"
#include "Configuration/Config.h"
#include "Master.h"

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

void usage(const char* prog)
{
    printf("Usage:\n");
    printf(" %s [<options>]\n", prog);
    printf("-c config_file (use config_file as configuration file)\n");
#ifdef _WIN32
    printf("    Running as service functions:\n");
    printf("    --service                run as service\n");
    printf("    -s install               install service\n");
    printf("    -s uninstall             uninstall service\n");
#endif
}

//Launch TUMORS
int main(int argc, const char * argv[])
{
    std::cout << argv[0] << std::endl;
    char const* cfg_file = _TUMORS_CORE_CONFIG;
    int *c = new int(1);
    while (*c < argc)
    {
        if (!strcmp(argv[*c], "-c"))
        {
            if (++*c >= argc)
            {
                printf("\nRuntime-Error: -c option requires an input argument.\n");
                usage(argv[0]);
                return 1;
            }
            else
                cfg_file = argv[*c];
        }
        
        //Just in-case we ever decide to use a Windows Server, let's get this working.
        #ifdef _WIN32
        if (strcmp(argv[*c], "-s") == 0)
        {
            printf("Services are not programmed yet. Shutting down the program...");
            return 1;
        }
        #endif
        ++*c;
    }
    delete c;
   
    if (!sConfig.LoadInitial(cfg_file))
    {
        printf("Invalid or missing configuration file : %s\n", cfg_file);
        printf("Verify that the file exists and has \'[TUMORS_CONFIGURATION_FILE]' written in the top of the file!\n");
        printf("NOTE: THIS ERROR ALSO OCCURS IF YOU DO NOT SET THE \"CD\" COMMAND IN TERMINAL TO THE DIRECTORY OF THE PROGRAM FIRST.\n");
        return 1;
    }
    
    UVO_LOG_INFO("server.worldserver", "%s::%s-%s-%s", _TUMORS_BANNER, CONFIG, PLATFORM_TEXT, ARCH);
    UVO_LOG_INFO("server.worldserver", "Using configuration file %s.", cfg_file);
    UVO_LOG_INFO("server.worldserver", "Using SSL Version: %s", OPENSSL_VERSION_TEXT);
    int *boostMajor = new int(BOOST_VERSION / 1000000);
    int *boostMinor = new int(BOOST_VERSION / 100 % 1000);
    int *boostSubMinor = new int(BOOST_VERSION % 100);
    UVO_LOG_INFO("server.worldserver", "Using BOOST Version: %i.%i.%i (Lib: %s)", *boostMajor, *boostMinor, *boostSubMinor, BOOST_LIB_VERSION);
    
    delete(boostMajor);
    delete(boostMinor);
    delete(boostSubMinor);
    
    /*Master *sMaster = new Master();
    int ret = sMaster->Run();
    
    return ret;*/
    return 0;
}

