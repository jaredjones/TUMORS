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
using boost::asio::streambuf;

WorldSocket::WorldSocket(tcp::socket&& socket)
: _socket(std::move(socket)), _authSeed(static_cast<uint32>(rand())), _OverSpeedPings(0), _worldSession(nullptr)
{
    
}

void WorldSocket::Start()
{
    AsyncReadHeader();
}

void WorldSocket::HandleSendAuthSession()
{
    
}

void WorldSocket::AsyncReadHeader()
{
    auto self(shared_from_this());
    _socket.async_read_some(boost::asio::buffer(_readBuffer, sizeof(ClientPktHeader)), [this, self](boost::system::error_code error, size_t transferedBytes)
    {
        if (!error)
        {
            for (int i = 0; i < transferedBytes; i++)
            {
                printf("%d:", _readBuffer[i]);
            }
            printf("\n");
            AsyncReadHeader();
        }
        else
        {
            // _socket.is_open() till returns true even after calling close()
            boost::system::error_code socketError;
            _socket.close(socketError);
        }
    });
}