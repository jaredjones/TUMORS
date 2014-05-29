#ifndef _TUMORS_LOGGER_H
#define _TUMORS_LOGGER_H

#include <string>

#include "Appender.h"

class Logger
{
public:
    Logger();
    
    void Create(std::string const& name, LogLevel level);
    void addAppender(uint8 type, Appender *);
    void delAppender(uint8 type);
    
    std::string const& getName() const;
    LogLevel getLogLevel() const;
    void setLogLevel(LogLevel level);
    void write(LogMessage& message) const;
private:
    std::string name;
    LogLevel level;
    AppenderMap appenders;
};

#endif