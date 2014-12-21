#ifndef _TUMORS_NetworkThread_h__
#define _TUMORS_NetworkThread_h__

#include "Errors.h"
#include "Log.h"
#include "Timer.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <set>
#include <thread>

template<class SocketType>
class NetworkThread
{
public:
    NetworkThread() : _connections(0), _stopped(false), _thread(nullptr)
    {
    }
    
    virtual ~NetworkThread()
    {
        Stop();
        if (_thread)
        {
            Wait();
            delete _thread;
        }
    }
    
    void Stop()
    {
        _stopped = true;
    }
    
    bool Start()
    {
        if (_thread)
            return false;
        
        _thread = new std::thread(&NetworkThread::Run, this);
            return true;
    }
    
    void Wait()
    {
        ASSERT(_thread);
        
        _thread->join();
        delete _thread;
        _thread = nullptr;
    }
    
    int32 GetConnectionCount() const
    {
        return _connections;
    }
    
    virtual void AddSocket(std::shared_ptr<SocketType> sock)
    {
        std::lock_guard<std::mutex> lock(_newSocketsLock);
        
        ++_connections;
        _newSockets.insert(sock);
        SocketAdded(sock);
    }
    
protected:
    virtual void SocketAdded(std::shared_ptr<SocketType> /*sock*/) { }
    virtual void SocketRemoved(std::shared_ptr<SocketType> /*sock*/) { }
    
    void AddNewSockets()
    {
        std::lock_guard<std::mutex> lock(_newSocketsLock);
        
        if (_newSockets.empty())
            return;
        
        for (typename SocketSet::const_iterator i = _newSockets.begin(); i != _newSockets.end(); ++i)
        {
            if (!(*i)->IsOpen())
            {
                SocketRemoved(*i);
                
                --_connections;
            }
            else
                _Sockets.insert(*i);
        }
        
        _newSockets.clear();
    }
    
    void Run()
    {
        UVO_LOG_DEBUG("misc", "Network Thread Starting");
        
        typename SocketSet::iterator i, t;
        
        uint32 sleepTime = 10;
        uint32 tickStart = 0, diff = 0;
        while (!_stopped)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            
            tickStart = getMSTime();
            
            AddNewSockets();
            
            for (i = _Sockets.begin(); i != _Sockets.end();)
            {
                if (!(*i)->Update())
                {
                    if ((*i)->IsOpen())
                        (*i)->CloseSocket();
                    
                    SocketRemoved(*i);
                    
                    --_connections;
                    _Sockets.erase(i++);
                }
                else
                    ++i;
            }
            
            diff = GetMSTimeDiffToNow(tickStart);
            sleepTime = diff > 10 ? 0 : 10 - diff;
        }
        
        UVO_LOG_DEBUG("misc", "Network Thread exits");
    }
    
private:
    typedef std::set<std::shared_ptr<SocketType> > SocketSet;
    
    std::atomic<int32> _connections;
    std::atomic<bool> _stopped;
    
    std::thread* _thread;
    
    SocketSet _Sockets;
    
    std::mutex _newSocketsLock;
    SocketSet _newSockets;
};

#endif