//
//  AsyncAcceptor.h
//  TUMORS
//
//  Created by Jared Jones on 7/23/14.
//
//

#ifndef TUMORS_AsyncAcceptor_h
#define TUMORS_AsyncAcceptor_h

#include "Log.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class AsyncAcceptor
{
public:
    typedef void(*ManagerAcceptHandler)(tcp::socket&& newSocket);
    
    AsyncAcceptor(boost::asio::io_service& ioService, std::string const& bindIp, uint16 port) :
    _acceptor(ioService, tcp::endpoint(boost::asio::ip::address::from_string(bindIp), port)),
    _socket(ioService)
    {
        //Nothing
    }
    
    template <class T>
    void AsyncAccept();
    
    void AsyncAcceptManaged(ManagerAcceptHandler mgrHandler)
    {
        _acceptor.async_accept(_socket, [this, mgrHandler](boost::system::error_code error)
        {
            if (!error)
            {
                try
                {
                    _socket.non_blocking(true);
                    mgrHandler(std::move(_socket));
                }
                catch (boost::system::system_error const& err)
                {
                    UVO_LOG_INFO("network", "Failed to initialize client's socket %s", err.what());
                }
        }
                                   
            AsyncAcceptManaged(mgrHandler);
        });
    }
    
private:
    tcp::acceptor _acceptor;
    tcp::socket _socket;
};

template<class T>
void AsyncAcceptor::AsyncAccept()
{
    _acceptor.async_accept(_socket, [this](boost::system::error_code error)
    {
        if (!error)
        {
            try
            {
                // this-> is required here to fix an segmentation fault in gcc 4.7.2 - reason is lambdas in a templated class
                std::make_shared<T>(std::move(this->_socket))->Start();
            }
            catch (boost::system::system_error const& err)
            {
                UVO_LOG_INFO("network", "Failed to retrieve client's remote address %s", err.what());
            }
        }
                               
        // lets slap some more this-> on this so we can fix this bug with gcc 4.7.2 throwing internals in yo face
        this->AsyncAccept<T>();
    });
}

#endif
