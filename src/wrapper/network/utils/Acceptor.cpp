/* boost.asio-wrapper, A C++ library, wrapping boost.asio, to be used for client or server applications
   Copyright (C) 2014 firecoders

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
   OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "Acceptor.h"
#include "wrapper/network/Connection_handler.h"

using namespace wrapper::network;

Acceptor::Acceptor(boost::asio::io_service& io_service, Connection_handler* connection_handler, int port):
        tcp_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        connection_handler(connection_handler), port(port)
{
    start_accept();
    boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));
}

void Acceptor::close(){
    tcp_acceptor.close();
}

void Acceptor::start_accept()
{
    boost::shared_ptr<Connection> connection = boost::shared_ptr<Connection>(new Connection(tcp_acceptor.get_io_service(), *connection_handler));

    tcp_acceptor.async_accept(connection->get_socket(),
                              boost::bind(&Acceptor::handle_accept, this, boost::asio::placeholders::error, connection));
}

void Acceptor::handle_accept(const boost::system::error_code& error, boost::shared_ptr<Connection>& connection){
    if(!error){
        connection_handler->add_connection(connection, EVENTS::CONNECTION_WAS_OPENED);

        start_accept();
    }
    else
        std::cerr << error.message() << std::endl;
}
