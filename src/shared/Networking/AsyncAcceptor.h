//
//  AsyncAcceptor.h
//  TUMORS
//
//  Created by Jared Jones on 7/23/14.
//
//

#ifndef TUMORS_AsyncAcceptor_h
#define TUMORS_AsyncAcceptor_h

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

template <class T>
class AsyncAcceptor
{
public:
    AsyncAcceptor(boost::asio::io_service& ioService, std::string bindIp, int port) :
    _acceptor(ioService, tcp::endpoint(boost::asio::ip::address::from_string(bindIp), port)),
    _socket(ioService)
    {
        AsyncAccept();
    };
    
    AsyncAcceptor(boost::asio::io_service& ioService, std::string bindIp, int port, bool tcpNoDelay) :
    _acceptor(ioService, tcp::endpoint(boost::asio::ip::address::from_string(bindIp), port)),
    _socket(ioService)
    {
        _acceptor.set_option(boost::asio::ip::tcp::no_delay(tcpNoDelay));
        
        AsyncAccept();
    };
    
private:
    void AsyncAccept()
    {
        _acceptor.async_accept(_socket, [this](boost::system::error_code error)
        {
            if (!error)
            {
                // this-> is required here to fix an segmentation fault in gcc 4.7.2 - reason is lambdas in a templated class
                    std::make_shared<T>(std::move(this->_socket))->Start();
            }
                                   
            // lets slap some more this-> on this so we can fix this bug with gcc 4.7.2 throwing internals in yo face
            this->AsyncAccept();
        });
    }
    
    tcp::acceptor _acceptor;
    tcp::socket _socket;
};

#endif
