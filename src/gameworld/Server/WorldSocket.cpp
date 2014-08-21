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
: Socket(std::move(socket), sizeof(ClientPktHeader)), _authSeed(rand()), _OverSpeedPings(0), _worldSession(nullptr)
{
}

void WorldSocket::Start()
{
    AsyncReadHeader();
}

void WorldSocket::HandleSendAuthSession()
{
    
}

void WorldSocket::ReadHeaderHandler()
{
    
}

void WorldSocket::ReadDataHandler()
{
    
}

void WorldSocket::CloseSocket()
{
    Socket::CloseSocket();
}
