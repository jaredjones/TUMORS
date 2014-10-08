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
    
    bool IsValidSize() const { return size >= 4 && size < 10240; }
    //bool IsValidOpcode() const { return cmd < NUM_MSG_TYPES; }
};

#pragma pack(pop)

class WorldSocket : public Socket<WorldSocket>
{
public:
    WorldSocket(tcp::socket&& socket);
    
    WorldSocket(WorldSocket const& right) = delete;
    WorldSocket& operator=(WorldSocket const& right) = delete;
    
    void Start() override;
    
    //void SendPacket(WorldPacket& packet);
    
protected:
    void ReadHandler() override;
    bool ReadHeaderHandler();
    bool ReadDataHandler();
    
private:
    void HandleSendAuthSession();
    //void HandleAuthSession(WorldPacket& recvPacket);
    void SendAuthResponseError(uint8 code);
    
    //void HandlePing(WorldPacket& recvPacket);
    
    uint32 _authSeed;
    //AuthCrypt _authCrypt;
    
    std::chrono::steady_clock::time_point _LastPingTime;
    uint32 _OverSpeedPings;
    
    void* _worldSession;
    //WorldSession* _worldSession;
    
    MessageBuffer _headerBuffer;
    MessageBuffer _packetBuffer;
};
#endif
