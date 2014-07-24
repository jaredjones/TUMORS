//
//  WorldSocket.h
//  TUMORS
//
//  Created by Jared Jones on 7/23/14.
//
//

#ifndef TUMORS_WorldSocket_h
#define TUMORS_WorldSocket_h

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

#include <Common.h>

using boost::asio::ip::tcp;

#pragma pack(push, 1)

struct ClientPktHeader
{
    uint16 size;
    uint32 cmd;
};

#pragma pack(pop)

class WorldSocket : public std::enable_shared_from_this<WorldSocket>
{
public:
    WorldSocket(tcp::socket&& socket);
    
    WorldSocket(WorldSocket const& right) = delete;
    WorldSocket& operator=(WorldSocket const& right) = delete;
    
    void Start();
    
    std::string GetRemoteIpAddress() const { return _socket.remote_endpoint().address().to_string(); };
    uint16 GetRemotePort() const { return _socket.remote_endpoint().port(); }
    
    void CloseSocket() { _socket.close(); };
    bool IsOpen() { return _socket.is_open(); };
    
    //void AsyncWrite(WorldPacket const& packet);
    
private:
    void HandleSendAuthSession();
    //void HandleAuthSession(WorldPacket& recvPacket);
    void SendAuthResponseError(uint8 code);
    
    //void HandlePing(WorldPacket& recvPacket);
    
    void AsyncReadHeader();
    void AsyncReadData(size_t dataSize);
    
    tcp::socket _socket;
    
    uint32 _authSeed;
    
    uint32 _OverSpeedPings;
    
    //Temporary befere session is coded
    void* _worldSession;
};
#endif
