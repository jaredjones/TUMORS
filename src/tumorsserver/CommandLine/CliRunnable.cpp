//
//  CliRunnable.cpp
//  TUMORS
//
//  Created by Jared Jones on 7/23/14.
//
//

#include "CliRunnable.h"
#include "World.h"

#if PLATFORM != PLATFORM_WINDOWS
#include <readline/readline.h>
#include <readline/history.h>

char* command_finder(const char* text, int state)
{
    static int idx, len;
    const char* ret;
    
    //tmp to supress warnings
    text = nullptr;
    state = 0;
    idx = 0;
    len = 0;
    ret = nullptr;
    
    return ((char*)NULL);
}

char** cli_completion(const char* text, int start, int /*end*/)
{
    char** matches = NULL;
    
    if (start)
        rl_bind_key('\t', rl_abort);
    else
        matches = rl_completion_matches((char*)text, &command_finder);
    return matches;
}

int cli_hook_func()
{
    if (World::IsStopped())
        rl_done = 1;
    return 0;
}

#endif

void CliThread()
{
    ///- Display the list of available CLI functions then beep
#if PLATFORM != PLATFORM_WINDOWS
    rl_attempted_completion_function = cli_completion;
    rl_event_hook = cli_hook_func;
#endif
    
    if (sConfig.GetBoolDefault("BeepAtStart", true))
        printf("\a");
    
    // print this here the first time
    // later it will be printed after command queue updates
    printf("TC>");
}