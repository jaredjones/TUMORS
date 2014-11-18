#include "Config.h"
#include "NetworkThread.h"
#include "WorldSocket.h"
#include "WorldSocketMgr.h"
#include <boost/system/error_code.hpp>

static void OnSocketAccept(tcp::socket&& sock)
{
    sWorldSocketMgr.OnSocketOpen(std::forward<tcp::socket>(sock));
}

class WorldSocketThread : public NetworkThread<WorldSocket>
{
public:
    void SocketAdded(std::shared_ptr<WorldSocket> sock) override
    {
        //sScriptMgr->OnSocketOpen(sock);
    }
    
    void SocketRemoved(std::shared_ptr<WorldSocket> sock) override
    {
        //sScriptMgr->OnSocketClose(sock);
    }
};

WorldSocketMgr::WorldSocketMgr() : BaseSocketMgr(), _socketSendBufferSize(-1), m_SockOutUBuff(65536), _tcpNoDelay(true)
{
}

bool WorldSocketMgr::StartNetwork(boost::asio::io_service& service, std::string const& bindIp, uint16 port)
{
    _tcpNoDelay = sConfig.GetBoolDefault("Network.TcpNodelay", true);
    
    UVO_LOG_DEBUG("misc", "Max allowed socket connections %d", boost::asio::socket_base::max_connections);
    
    // -1 means use default
    _socketSendBufferSize = sConfig.GetIntDefault("Network.OutKBuff", -1);
    
    m_SockOutUBuff = sConfig.GetIntDefault("Network.OutUBuff", 65536);
    
    if (m_SockOutUBuff <= 0)
    {
        UVO_LOG_ERROR("misc", "Network.OutUBuff is wrong in your config file");
        return false;
    }
    
    BaseSocketMgr::StartNetwork(service, bindIp, port);
    
    _acceptor->AsyncAcceptManaged(&OnSocketAccept);
    
    //sScriptMgr->OnNetworkStart();
    return true;
}

void WorldSocketMgr::StopNetwork()
{
    BaseSocketMgr::StopNetwork();
    
    //sScriptMgr->OnNetworkStop();
}

void WorldSocketMgr::OnSocketOpen(tcp::socket&& sock)
{
    // set some options here
    if (_socketSendBufferSize >= 0)
    {
        boost::system::error_code err;
        sock.set_option(boost::asio::socket_base::send_buffer_size(_socketSendBufferSize), err);
        if (err && err != boost::system::errc::not_supported)
        {
            UVO_LOG_ERROR("misc", "WorldSocketMgr::OnSocketOpen sock.set_option(boost::asio::socket_base::send_buffer_size) err = %s", err.message().c_str());
            return;
        }
    }
    
    // Set TCP_NODELAY.
    if (_tcpNoDelay)
    {
        boost::system::error_code err;
        sock.set_option(boost::asio::ip::tcp::no_delay(true), err);
        if (err)
        {
            UVO_LOG_ERROR("misc", "WorldSocketMgr::OnSocketOpen sock.set_option(boost::asio::ip::tcp::no_delay) err = %s", err.message().c_str());
            return;
        }
    }
    
    //sock->m_OutBufferSize = static_cast<size_t> (m_SockOutUBuff);
    
    BaseSocketMgr::OnSocketOpen(std::forward<tcp::socket>(sock));
}

NetworkThread<WorldSocket>* WorldSocketMgr::CreateThreads() const
{
    return new WorldSocketThread[GetNetworkThreadCount()];
}