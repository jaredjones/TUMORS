//
//  WorldSocket.h
//  TUMORS
//
//  Created by Jared Jones on 7/23/14.
//
//

#ifndef TUMORS_WorldSocket_h
#define TUMORS_WorldSocket_h

#include <chrono>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>

#include "Common.h"
#include "Socket.h"

using boost::asio::ip::tcp;

#pragma pack(push, 1)

struct ClientPktHeader
{
    uint16 size;
    uint32 cmd;
    //bool isValid() const { return size >= 4 && size < 10240 && cmd < NUM_MSG_TYPES; }
};

#pragma pack(pop)

struct WorldPacketBuffer
{
    typedef boost::asio::const_buffer value_type;
    typedef boost::asio::const_buffer const* const_iterator;
    
};

class WorldSocket : public Socket<WorldSocket, WorldPacketBuffer>
{
    typedef Socket<WorldSocket, WorldPacketBuffer> Base;
public:
    WorldSocket(tcp::socket&& socket);
    
    WorldSocket(WorldSocket const& right) = delete;
    WorldSocket& operator=(WorldSocket const& right) = delete;
    
    void Start() override;
    
    void CloseSocket() override;
    
    using Base::AsyncWrite;
    //void AsyncWrite(WorldPacket& packet);
    
protected:
    void ReadHeaderHandler() override;
    void ReadDataHandler() override;
    
private:
    void HandleSendAuthSession();
    //void HandleAuthSession(WorldPacket& recvPacket);
    void SendAuthResponseError(uint8 code);
    
    //void HandlePing(WorldPacket& recvPacket);
    
    uint32 _authSeed;
    
    std::chrono::steady_clock::time_point _LastPingTime;
    uint32 _OverSpeedPings;
    
    //Temporary befere session is coded
    void* _worldSession;
};
#endif
