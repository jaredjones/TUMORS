//
//  Config.cpp
//  TUMORS
//
//  Created by System Administrator on 9/27/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#include "Config.h"
#include <boost/algorithm/string.hpp>

bool ConfigMgr::LoadInitial(char const* file)
{
    GuardType guard(_configLock);
    _configMap.clear();
    _configLineList.clear();
    
    _filename = file;
    if( !(access( _filename.c_str(), F_OK ) != -1) )
    {
        boost::filesystem::path fullLocalPath(boost::filesystem::current_path());
    
    #ifdef _WIN32
        _filename = fullLocalPath.string() + "\\" + _filename;
    #else
        _filename = fullLocalPath.string() + "/" + _filename;
    #endif
    }
    std::ifstream infile(_filename);
    
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
            bool containsWhiteSpace = true;
            bool hasHitEqualSign = false;
            for (unsigned int i = 0; i < line.length(); i++)
            {
                if (!isspace(line[i]))
                {
                    containsWhiteSpace = false;
                }
                if (!containsWhiteSpace)
                {
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
            }
            
            _configMap.insert(std::make_pair<std::string, std::string>(ssKey.str(), ssValue.str()));
            
            if (containsWhiteSpace)
                continue;
            
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
        for (uint64 i = static_cast<uint64>(pos); i > 0; i--)
        {
            if (!isspace(s[i-1]))
                nonWhiteFound = true;
        }
        if (nonWhiteFound)
            continue;
        
        std::stringstream ss;
        for (uint64 i = static_cast<uint64>(pos); i < s.length(); i++)
        {
            if (i == static_cast<uint64>(pos))
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
    
    std::string tmp = boost::to_upper_copy((std::string)_configMap.at(name));
    return (tmp == "1" || tmp == "YES" || tmp == "TRUE" );
}