#include <time.h>
#include <stdio.h>

#include "Appender.h"

std::string LogMessage::getTimeStr(time_t time)
{
    //time_t t = time(NULL);
    tm *timePtr = localtime(&time);

    char buf[20];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d_%02d:%02d:%02d", timePtr->tm_year+1900, timePtr->tm_mon+1,
         timePtr->tm_mday, timePtr->tm_hour,
         timePtr->tm_min, timePtr->tm_sec);
    return std::string(buf);
}

std::string LogMessage::getTimeStr()
{
    return getTimeStr(mtime);
}

Appender::Appender(uint8 _id, std::string const& _name, AppenderType _type /* = APPENDER_NONE*/, LogLevel _level /* = LOG_LEVEL_DISABLED */, AppenderFlags _flags /* = APPENDER_FLAGS_NONE */):
id(_id), name(_name), type(_type), level(_level), flags(_flags) { }

Appender::~Appender() { }

uint8 Appender::getId() const
{
    return id;
}

std::string const& Appender::getName() const
{
    return name;
}

AppenderType Appender::getType() const
{
    return type;
}

LogLevel Appender::getLogLevel() const
{
    return level;
}

AppenderFlags Appender::getFlags() const
{
    return flags;
}

void Appender::setLogLevel(LogLevel _level)
{
    level = _level;
}

void Appender::write(LogMessage& message)
{
    if (!level || level > message.level)
        return;
    
    message.prefix.clear();
    if (flags & APPENDER_FLAGS_PREFIX_TIMESTAMP)
        message.prefix.append(message.getTimeStr());
    
    if (flags & APPENDER_FLAGS_PREFIX_LOGLEVEL)
    {
        if (!message.prefix.empty())
            message.prefix.push_back(' ');
        
        char text[MAX_QUERY_LEN];
        snprintf(text, MAX_QUERY_LEN, "%-5s", Appender::getLogLevelString(message.level));
        message.prefix.append(text);
    }
    
    if (flags & APPENDER_FLAGS_PREFIX_LOGFILTERTYPE)
    {
        if (!message.prefix.empty())
            message.prefix.push_back(' ');
        
        message.prefix.push_back('[');
        message.prefix.append(message.type);
        message.prefix.push_back(']');
    }
    
    if (!message.prefix.empty())
        message.prefix.push_back(' ');
    
    _write(message);
}

const char* Appender::getLogLevelString(LogLevel level)
{
    switch (level)
    {
        case LOG_LEVEL_FATAL:
            return "FATAL";
        case LOG_LEVEL_ERROR:
            return "ERROR";
        case LOG_LEVEL_WARN:
            return "WARN";
        case LOG_LEVEL_INFO:
            return "INFO";
        case LOG_LEVEL_DEBUG:
            return "DEBUG";
        case LOG_LEVEL_TRACE:
            return "TRACE";
        default:
            return "DISABLED";
    }
}