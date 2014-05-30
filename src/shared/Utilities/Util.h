//
//  Util.h
//  TUMORS
//
//  Created by Jared Jones on 12/26/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#ifndef TUMORS_Util_h
#define TUMORS_Util_h

#include "Common.h"

uint32 CreatePIDFile(const std::string& filename);

class Tokenizer
{
public:
    typedef std::vector<char const*> StorageType;
    
    typedef StorageType::size_type size_type;
    
    typedef StorageType::const_iterator const_iterator;
    typedef StorageType::reference reference;
    typedef StorageType::const_reference const_reference;
    
public:
    Tokenizer(const std::string &src, char const sep, uint32 vectorReserve = 0);
    ~Tokenizer() { delete[] m_str; }
    
    const_iterator begin() const { return m_storage.begin(); }
    const_iterator end() const { return m_storage.end(); }
    
    size_type size() const { return m_storage.size(); }
    
    reference operator [] (size_type i) { return m_storage[i]; }
    const_reference operator [] (size_type i) const { return m_storage[i]; }
    
private:
    char* m_str;
    StorageType m_storage;
};

void utf8printf(FILE* out, const char *str, ...);
void vutf8printf(FILE* out, const char *str, va_list* ap);

// UTF8 handling
bool Utf8toWStr(const std::string& utf8str, std::wstring& wstr);
// in wsize==max size of buffer, out wsize==real string size
bool Utf8toWStr(char const* utf8str, size_t csize, wchar_t* wstr, size_t& wsize);
inline bool Utf8toWStr(const std::string& utf8str, wchar_t* wstr, size_t& wsize)
{
	return Utf8toWStr(utf8str.c_str(), utf8str.size(), wstr, wsize);
}

bool WStrToUtf8(std::wstring wstr, std::string& utf8str);
// size==real string size
bool WStrToUtf8(wchar_t* wstr, size_t size, std::string& utf8str);

size_t utf8length(std::string& utf8str);                    // set string to "" if invalid utf8 sequence
void utf8truncate(std::string& utf8str, size_t len);

#endif
