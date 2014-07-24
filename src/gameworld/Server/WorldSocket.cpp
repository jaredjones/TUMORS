//
//  WorldSocket.cpp
//  TUMORS
//
//  Created by Jared Jones on 7/23/14.
//
//

#include "WorldSocket.h"

using boost::asio::ip::tcp;
using boost::asio::streambuf;

WorldSocket::WorldSocket(tcp::socket&& socket)
: _socket(std::move(socket)), _authSeed(static_cast<uint32>(rand())), _OverSpeedPings(0), _worldSession(nullptr)
{
    printf("ISOPEN:%d\n",_socket.is_open());
}

void WorldSocket::Start()
{
    printf("ISOPEN:%d\n",_socket.is_open());
}

void WorldSocket::HandleSendAuthSession()
{
    
}

void WorldSocket::AsyncReadHeader()
{
    
}