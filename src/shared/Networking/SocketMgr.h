#ifndef _TUMORS_SocketMGR_H
#define _TUMORS_SocketMGR_H

#include "Common.h"
#include "AsyncAcceptor.h"
#include "Config.h"
#include "NetworkThread.h"
#include <boost/asio/ip/tcp.hpp>
#include <memory>

using boost::asio::ip::tcp;

template<class SocketType>
class SocketMgr
{
public:
    virtual ~SocketMgr()
    {
        delete[] _threads;
    }
    
    virtual bool StartNetwork(boost::asio::io_service& service, std::string const& bindIp, uint16 port)
    {
        _threadCount = sConfig.GetIntDefault("Network.Threads", 1);
        
        if (_threadCount <= 0)
        {
            UVO_LOG_ERROR("misc", "Network.Threads is wrong in your config file");
            return false;
        }
        
        _acceptor = new AsyncAcceptor(service, bindIp, port);
        _threads = CreateThreads();
        
        //NEEDTOFIX ASSERT DOESN"T EXIST IN THIS CONTEXT
        //ASSERT(_threads);
        
        for (int32 i = 0; i < _threadCount; ++i)
            _threads[i].Start();
        
        return true;
    }
    
    virtual void StopNetwork()
    {
        if (_threadCount != 0)
            for (int32 i = 0; i < _threadCount; ++i)
                _threads[i].Stop();
        
        Wait();
    }
    
    void Wait()
    {
        if (_threadCount != 0)
            for (int32 i = 0; i < _threadCount; ++i)
                _threads[i].Wait();
    }
    
    virtual void OnSocketOpen(tcp::socket&& sock)
    {
        size_t min = 0;
        
        for (int32 i = 1; i < _threadCount; ++i)
            if (_threads[i].GetConnectionCount() < _threads[min].GetConnectionCount())
                min = i;
        
        try
        {
            std::shared_ptr<SocketType> newSocket = std::make_shared<SocketType>(std::move(sock));
            newSocket->Start();
            
            _threads[min].AddSocket(newSocket);
        }
        catch (boost::system::system_error const& err)
        {
            UVO_LOG_INFO("network", "Failed to retrieve client's remote address %s", err.what());
        }
    }
    
    int32 GetNetworkThreadCount() const { return _threadCount; }
    
protected:
    SocketMgr() : _acceptor(nullptr), _threads(nullptr), _threadCount(1)
    {
    }
    
    virtual NetworkThread<SocketType>* CreateThreads() const = 0;
    
    AsyncAcceptor* _acceptor;
    NetworkThread<SocketType>* _threads;
    int32 _threadCount;
};

#endif