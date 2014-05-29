//
//  Logging.h
//  TUMORS
//
//  Created by System Administrator on 9/27/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#ifndef __TUMORS__Logging__
#define __TUMORS__Logging__

#include <iostream>
#include <string>
#include <unordered_map>

#include "Common.h"
#include "Logger.h"
#include "Appender.h"

#define sLog Log::getInstance()

////the following are UBUNTU/LINUX/BSD ONLY terminal color codes.
//#define RESET   "\033[0m"
//#define BLACK   "\033[30m"      /* Black */
//#define RED     "\033[31m"      /* Red */
//#define GREEN   "\033[32m"      /* Green */
//#define YELLOW  "\033[33m"      /* Yellow */
//#define BLUE    "\033[34m"      /* Blue */
//#define MAGENTA "\033[35m"      /* Magenta */
//#define CYAN    "\033[36m"      /* Cyan */
//#define WHITE   "\033[37m"      /* White */
//#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
//#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
//#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
//#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
//#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
//#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
//#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
//#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define LOGGER_ROOT "root"

static const std::string AppenderPrepender = "Appender.";
static const std::string LoggerPrepender = "Logger.";

class Log
{
    typedef std::unordered_map<std::string, Logger> LoggerMap;
    
private:
    Log();
    ~Log();
    static std::string GetTimestampStr();
    void vlog(std::string const& f, LogLevel level, char const* str, va_list argptr);
    void write(LogMessage *msg) const;
    
    Logger const* GetLoggerByType(std::string const &origionalType) const;
    Appender* GetAppenderByName(std::string const& name);
    uint8 NextAppenderId();
    void CreateAppenderFromConfig(std::string const& appenderName);
    void CreateLoggerFromConfig(std::string const& appenderName);
    void ReadAppendersFromConfig();
    void ReadLoggersFromConfig();
    
    AppenderMap appenders;
    LoggerMap loggers;
    uint8 AppenderId;
    
    std::string m_logsDir;
    std::string m_logsTimestamp;
    
public:
    void LoadFromConfig();
    void Close();
    bool ShouldLog(std::string const& type, LogLevel level);
    bool SetLogLevel(std::string const& name, char const* level, bool isLogger = true);
    
    void outMessage(std::string const& f, LogLevel level, char const* str, ...) ATTR_PRINTF(4, 5);
    
    void outCommand(uint32 account, const char * str, ...) ATTR_PRINTF(3, 4);
    void outCharDump(char const* str, uint32 account_id, uint32 guid, char const* name);
    
    static Log& getInstance()
    {
        //C++11 way of creating a thread-safe lazy instantiation for singletons
        static Log singleObject;
        return singleObject;
    }
    
    
};

inline Logger const* Log::GetLoggerByType(std::string const& origionalType) const
{
    LoggerMap::const_iterator itCached = loggers.find(origionalType);
    if (itCached != loggers.end())
        return &(itCached->second);
    
    if (origionalType == LOGGER_ROOT)
        return NULL;
    
    std::string parentLogger = LOGGER_ROOT;
    size_t found = origionalType.find_last_of(".");
    if (found != std::string::npos)
        parentLogger = origionalType.substr(0, found);
    
    return GetLoggerByType(parentLogger);
}

inline bool Log::ShouldLog(std::string const& type, LogLevel level)
{
    Logger const *logger = GetLoggerByType(type);
    if (!logger)
        return false;
    
    LogLevel logLevel = logger->getLogLevel();
    return logLevel != LOG_LEVEL_DISABLED && logLevel <= level;
}

inline void Log::outMessage(std::string const& filter, LogLevel level, const char *str, ...)
{
    va_list ap;
    va_start(ap, str);
    
    vlog(filter, level, str, ap);
    
    va_end(ap);
}

#if COMPILER != COMPILER_MICROSOFT
#define UVO_LOG_MESSAGE_BODY(filterType__, level__, ...)                 \
        do {                                                            \
            if (sLog.ShouldLog(filterType__, level__))                 \
                sLog.outMessage(filterType__, level__, __VA_ARGS__);   \
            } while (0)
#else
#define UVO_LOG_MESSAGE_BODY(filterType__, level__, ...)                 \
        __pragma(warning(push))                                         \
        __pragma(warning(disable:4127))                                 \
        do {                                                            \
            if (sLog.ShouldLog(filterType__, level__))                 \
                sLog.outMessage(filterType__, level__, __VA_ARGS__);   \
        } while (0)                                                     \
            __pragma(warning(pop))
#endif

#define UVO_LOG_TRACE(filterType__, ...) \
UVO_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_TRACE, __VA_ARGS__)

#define UVO_LOG_DEBUG(filterType__, ...) \
UVO_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_DEBUG, __VA_ARGS__)

#define UVO_LOG_INFO(filterType__, ...)  \
UVO_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_INFO, __VA_ARGS__)

#define UVO_LOG_WARN(filterType__, ...)  \
UVO_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_WARN, __VA_ARGS__)

#define UVO_LOG_ERROR(filterType__, ...) \
UVO_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_ERROR, __VA_ARGS__)

#define UVO_LOG_FATAL(filterType__, ...) \
UVO_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_FATAL, __VA_ARGS__)

#endif /* defined(__TUMORS__Logging__) */
