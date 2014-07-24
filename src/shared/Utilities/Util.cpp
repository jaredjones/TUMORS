//
//  Util.cpp
//  TUMORS
//
//  Created by Jared Jones on 12/26/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#include "Util.h"
#include "utf8.h"
//#include "unistd.h"

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

bool utf8ToConsole(const std::string& utf8str, std::string& conStr)
{
#if PLATFORM == PLATFORM_WINDOWS
    std::wstring wstr;
    if (!Utf8toWStr(utf8str, wstr))
        return false;
    
    conStr.resize(wstr.size());
    CharToOemBuffW(&wstr[0], &conStr[0], wstr.size());
#else
    // not implemented yet
    conStr = utf8str;
#endif
    
    return true;
}

bool consoleToUtf8(const std::string& conStr, std::string& utf8str)
{
#if PLATFORM == PLATFORM_WINDOWS
    std::wstring wstr;
    wstr.resize(conStr.size());
    OemToCharBuffW(&conStr[0], &wstr[0], conStr.size());
    
    return WStrToUtf8(wstr, utf8str);
#else
    // not implemented yet
    utf8str = conStr;
    return true;
#endif
}

bool Utf8FitTo(const std::string& str, std::wstring search)
{
    std::wstring temp;
    
    if (!Utf8toWStr(str, temp))
        return false;
    
    // converting to lower case
    wstrToLower( temp );
    
    if (temp.find(search) == std::wstring::npos)
        return false;
    
    return true;
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

size_t utf8length(std::string& utf8str)
{
	try
	{
		return utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size());
	}
	catch (std::exception)
	{
		utf8str = "";
		return 0;
	}
}

void utf8truncate(std::string& utf8str, size_t len)
{
	try
	{
		size_t wlen = utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size());
		if (wlen <= len)
			return;

		std::wstring wstr;
		wstr.resize(wlen);
		utf8::utf8to16(utf8str.c_str(), utf8str.c_str() + utf8str.size(), &wstr[0]);
		wstr.resize(len);
		char* oend = utf8::utf16to8(wstr.c_str(), wstr.c_str() + wstr.size(), &utf8str[0]);
		utf8str.resize(oend - (&utf8str[0]));                 // remove unused tail
	}
	catch (std::exception)
	{
		utf8str = "";
	}
}

bool Utf8toWStr(char const* utf8str, size_t csize, wchar_t* wstr, size_t& wsize)
{
	try
	{
		size_t len = utf8::distance(utf8str, utf8str + csize);
		if (len > wsize)
		{
			if (wsize > 0)
				wstr[0] = L'\0';
			wsize = 0;
			return false;
		}

		wsize = len;
		utf8::utf8to16(utf8str, utf8str + csize, wstr);
		wstr[len] = L'\0';
	}
	catch (std::exception)
	{
		if (wsize > 0)
			wstr[0] = L'\0';
		wsize = 0;
		return false;
	}

	return true;
}

bool Utf8toWStr(const std::string& utf8str, std::wstring& wstr)
{
	try
	{
		if (size_t len = utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size()))
		{
			wstr.resize(len);
			utf8::utf8to16(utf8str.c_str(), utf8str.c_str() + utf8str.size(), &wstr[0]);
		}
	}
	catch (std::exception)
	{
		wstr = L"";
		return false;
	}

	return true;
}

bool WStrToUtf8(wchar_t* wstr, size_t size, std::string& utf8str)
{
	try
	{
		std::string utf8str2;
		utf8str2.resize(size * 4);                            // allocate for most long case

		if (size)
		{
			char* oend = utf8::utf16to8(wstr, wstr + size, &utf8str2[0]);
			utf8str2.resize(oend - (&utf8str2[0]));               // remove unused tail
		}
		utf8str = utf8str2;
	}
	catch (std::exception)
	{
		utf8str = "";
		return false;
	}

	return true;
}

bool WStrToUtf8(std::wstring wstr, std::string& utf8str)
{
	try
	{
		std::string utf8str2;
		utf8str2.resize(wstr.size() * 4);                     // allocate for most long case

		if (wstr.size())
		{
			char* oend = utf8::utf16to8(wstr.c_str(), wstr.c_str() + wstr.size(), &utf8str2[0]);
			utf8str2.resize(oend - (&utf8str2[0]));                // remove unused tail
		}
		utf8str = utf8str2;
	}
	catch (std::exception)
	{
		utf8str = "";
		return false;
	}

	return true;
}