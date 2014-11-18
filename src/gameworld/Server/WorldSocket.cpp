//
//  WorldSocket.cpp
//  TUMORS
//
//  Created by Jared Jones on 7/23/14.
//
//
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>

#include "Log.h"
#include "WorldSocket.h"

using boost::asio::ip::tcp;

WorldSocket::WorldSocket(tcp::socket&& socket)
: Socket(std::move(socket)), _authSeed(rand()), _OverSpeedPings(0), _worldSession(nullptr)
{
    _headerBuffer.Resize(sizeof(ClientPktHeader));
}

void WorldSocket::Start()
{
    AsyncRead();
    HandleSendAuthSession();
}

void WorldSocket::HandleSendAuthSession()
{
    
}

void WorldSocket::ReadHandler()
{
    if (!IsOpen())
        return;
}

bool WorldSocket::ReadHeaderHandler()
{
    return true;
}

bool WorldSocket::ReadDataHandler()
{
    return true;
}

void WorldSocket::SendAuthResponseError(uint8 code)
{
    
}

