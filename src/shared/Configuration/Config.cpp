//
//  Config.cpp
//  TUMORS
//
//  Created by System Administrator on 9/27/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#include <algorithm>

#include "Config.h"

bool ConfigMgr::LoadInitial(char const* file)
{
    GuardType guard(_configLock);
    _configMap.clear();
    _configLineList.clear();
    
    _filename = file;
    
    std::ifstream infile(file);
    
    if(infile)
    {
        //Load Config File into a linked list.
        //Load Config File into Key/Value Unordered Map as well.
        std::string line;
        while (std::getline(infile, line))
        {
            if (line[0] == '#')
                continue;
            if (line.empty())
                continue;
            
            std::stringstream ssKey;
            std::stringstream ssValue;
            bool hasHitEqualSign = false;
            bool hasHitFirstValueCharAfterEqualSign = false;
            for (unsigned int i = 0; i < line.length(); i++)
            {
                if (isspace(line[i]) && !hasHitFirstValueCharAfterEqualSign)
                    continue;
                
                if (!isspace(line[i]) && !hasHitFirstValueCharAfterEqualSign && hasHitEqualSign)
                    hasHitFirstValueCharAfterEqualSign = true;
                
                if (line[i] == '=')
                {
                    hasHitEqualSign = true;
                    continue;
                }
                if (!hasHitEqualSign)
                    ssKey << line[i];
                else
                {
                        char tmp = line[i];
                        if (tmp == '\n' || tmp == '\r' || tmp == '\0' || tmp == '\v' || tmp == '\f')
                            break;
                        ssValue << tmp;
                }
                
            }
            
            _configMap.insert(std::make_pair<std::string, std::string>(ssKey.str(), ssValue.str()));
            
            if (!line.empty())
                _configLineList.push_back(line);
        }
        
        return true;
    }
    return false;
}

bool ConfigMgr::Reload()
{
    return LoadInitial(_filename.c_str());
}

std::string const& ConfigMgr::GetFilename()
{
    GuardType guard(_configLock);
    return _filename;
}

std::list<std::string> ConfigMgr::GetKeysByString(std::string const &name)
{
    GuardType guard(_configLock);
    
    std::list<std::string> keys;
    
    for (std::string s : _configLineList)
    {
        int64 pos = s.find(name);
        if (pos < 0)
            continue;
        
        bool nonWhiteFound = false;
        for (uint32 i = static_cast<uint32>(pos); i > 0; i--)
        {
            if (!isspace(s[i-1]))
                nonWhiteFound = true;
        }
        if (nonWhiteFound)
            continue;
        
        std::stringstream ss;
        for (uint32 i = static_cast<uint32>(pos); i < s.length(); i++)
        {
            if (i == static_cast<uint32>(pos))
            {
                ss.clear();
                ss.str(std::string());
            }
            
            if (s[i] == '=' || isspace(s[i]))
                break;
            
            ss << s[i];
        }
        keys.push_back(ss.str());
    }
    
    return keys;
}

std::string ConfigMgr::GetStringDefault(const char* name, const std::string &def)
{
    GuardType guard(_configLock);
    
    std::unordered_map<std::string,std::string>::const_iterator got = _configMap.find (name);
    if (got == _configMap.end())
        return def;
    
    std::string tmp = _configMap[name];
    return tmp;
}

bool ConfigMgr::GetBoolDefault(const char* name, bool def)
{
    GuardType guard(_configLock);
    
    std::unordered_map<std::string,std::string>::const_iterator got = _configMap.find (name);
    if (got == _configMap.end())
        return def;
    
    std::string tmp = (std::string)_configMap.at(name);
    std::transform(tmp.begin(), tmp.end(),tmp.begin(), ::toupper);
    return (tmp == "1" || tmp == "YES" || tmp == "TRUE" );
}

int ConfigMgr::GetIntDefault(const char* name, int def)
{
    GuardType guard(_configLock);
    
    std::unordered_map<std::string,std::string>::const_iterator got = _configMap.find (name);
    if (got == _configMap.end())
        return def;
    
    return atoi(_configMap.at(name).c_str());
}