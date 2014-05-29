//
//  Util.cpp
//  TUMORS
//
//  Created by Jared Jones on 12/26/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#include "Util.h"
#include "unistd.h"

Tokenizer::Tokenizer(const std::string &src, const char sep, uint32 vectorReserve)
{
    m_str = new char[src.length() + 1];
    memcpy(m_str, src.c_str(), src.length() + 1);
    
    if (vectorReserve)
        m_storage.reserve(vectorReserve);
    
    char* posold = m_str;
    char* posnew = m_str;
    
    while(1)
    {
        if (*posnew == sep)
        {
            m_storage.push_back(posold);
            posold = posnew + 1;
            
            *posnew = '\0';
        }
        else if (*posnew == '\0')
        {
            // Hack like, but the old code accepted these kind of broken strings,
            // so changing it would break other things
            if (posold != posnew)
                m_storage.push_back(posold);
            
            break;
        }
        
        ++posnew;
    }
}

uint32 CreatePIDFile(const std::string& filename)
{
    FILE* pid_file = fopen (filename.c_str(), "w" );
    if (pid_file == NULL)
        return 0;
    
#ifdef _WIN32
    DWORD pid = GetCurrentProcessId();
#else
    pid_t pid = getpid();
#endif
    
    fprintf(pid_file, "%u", pid );
    fclose(pid_file);
    
    return (uint32)pid;
}

void utf8printf(FILE* out, const char *str, ...)
{
    va_list ap;
    va_start(ap, str);
    vutf8printf(out, str, &ap);
    va_end(ap);
}

void vutf8printf(FILE* out, const char *str, va_list* ap)
{
#if PLATFORM == PLATFORM_WINDOWS
    char temp_buf[32*1024];
    wchar_t wtemp_buf[32*1024];
    
    size_t temp_len = vsnprintf(temp_buf, 32*1024, str, *ap);
    //vsnprintf returns -1 if the buffer is too small
    if (temp_len == size_t(-1))
        temp_len = 32*1024-1;
    
    size_t wtemp_len = 32*1024-1;
    Utf8toWStr(temp_buf, temp_len, wtemp_buf, wtemp_len);
    
    CharToOemBuffW(&wtemp_buf[0], &temp_buf[0], wtemp_len+1);
    fprintf(out, "%s", temp_buf);
#else
    vfprintf(out, str, *ap);
#endif
}