//
//  Config.h
//  TUMORS
//
//  Created by System Administrator on 9/27/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#ifndef TUMORS_Config_h
#define TUMORS_Config_h

#include <mutex>
#include <fstream>
#include <string>
#include <map>
#include <list>

#include "Common.h"
#include "Log.h"

#define sConfig ConfigMgr::getInstance()

class ConfigMgr
{
    
public:
    
    ConfigMgr(){}
    ~ConfigMgr(){}
    //Method used only for loading main's config file.
    bool LoadInitial(char const* file);
    bool Reload();
    std::string const& GetFilename();
    std::string GetStringDefault(const char* name, const std::string &def);
    bool GetBoolDefault(const char* name, bool def);
    int GetIntDefault(const char* name, int def);
    std::list<std::string> GetKeysByString(std::string const &name);
    
    static ConfigMgr& getInstance()
    {
        //C++11 way of creating a thread-safe lazy instantiation for singletons
        static ConfigMgr singleObject;
        return singleObject;
    }
    
private:
    typedef std::mutex LockType;
    typedef std::lock_guard<LockType> GuardType;
    
    LockType _configLock;
    
    std::string _filename;
    std::unordered_map<std::string, std::string> _configMap;
    std::list<std::string> _configLineList;
    
};

#endif
