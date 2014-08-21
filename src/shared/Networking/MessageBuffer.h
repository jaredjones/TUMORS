//
//  MessageBuffer.h
//  TUMORS
//
//  Created by Jared Jones on 8/21/14.
//
//

#ifndef TUMORS_MessageBuffer_h
#define TUMORS_MessageBuffer_h

#include <vector>

class MessageBuffer
{
    typedef std::vector<uint8>::size_type size_type;
    
public:
    MessageBuffer() : _wpos(0), _storage() { }
    
    MessageBuffer(MessageBuffer const& right) : _wpos(right._wpos), _storage(right._storage) { }
    
    MessageBuffer(MessageBuffer&& right) : _wpos(right._wpos), _storage(right.Move()) { }
    
    void Reset()
    {
        _storage.clear();
        _wpos = 0;
    }
    
    bool IsMessageReady() const { return _wpos == _storage.size(); }
    
    size_type GetSize() const { return _storage.size(); }
    
    size_type GetReadyDataSize() const { return _wpos; }
    
    size_type GetMissingSize() const { return _storage.size() - _wpos; }
    
    uint8* Data() { return _storage.data(); }
    
    void Grow(size_type bytes)
    {
        _storage.resize(_storage.size() + bytes);
    }
    
    uint8* GetWritePointer() { return &_storage[_wpos]; }
    
    void WriteCompleted(size_type bytes) { _wpos += bytes; }
    
    void ResetWritePointer() { _wpos = 0; }
    
    std::vector<uint8>&& Move()
    {
        _wpos = 0;
        return std::move(_storage);
    }
    
    MessageBuffer& operator=(MessageBuffer& right)
    {
        if (this != &right)
        {
            _wpos = right._wpos;
            _storage = right._storage;
        }
        
        return *this;
    }
    
    MessageBuffer& operator=(MessageBuffer&& right)
    {
        if (this != &right)
        {
            _wpos = right._wpos;
            _storage = right.Move();
        }
        
        return *this;
    }
    
private:
    size_type _wpos;
    std::vector<uint8> _storage;
};

#endif
