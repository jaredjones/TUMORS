#ifndef __TUMORS_WORLDSOCKETMGR_H
#define __TUMORS_WORLDSOCKETMGR_H

#include "SocketMgr.h"

class WorldSocket;

/// Manages all sockets connected to peers and network threads
class WorldSocketMgr : public SocketMgr<WorldSocket>
{
    typedef SocketMgr<WorldSocket> BaseSocketMgr;
    
public:
    static WorldSocketMgr& Instance()
    {
        static WorldSocketMgr instance;
        return instance;
    }
    
    /// Start network, listen at address:port .
    bool StartNetwork(boost::asio::io_service& service, std::string const& bindIp, uint16 port) override;
    
    /// Stops all network threads, It will wait for all running threads .
    void StopNetwork() override;
    
    void OnSocketOpen(tcp::socket&& sock) override;
    
protected:
    WorldSocketMgr();
    
    NetworkThread<WorldSocket>* CreateThreads() const override;
    
private:
    int32 _socketSendBufferSize;
    int32 m_SockOutUBuff;
    bool _tcpNoDelay;
};

#define sWorldSocketMgr WorldSocketMgr::Instance()

#endif