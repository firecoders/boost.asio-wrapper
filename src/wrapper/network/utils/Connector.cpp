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

#include "Connector.h"
#include "wrapper/network/Connection_handler.h"

using namespace wrapper::network;

Connector::Connector(boost::asio::io_service& io_service, Connection_handler* connection_handler, std::string ip, int port):
        connection(new Connection(io_service, *connection_handler)), connection_handler(connection_handler), ip(ip), port(port)
{
    boost::asio::ip::tcp::resolver::query query(ip, boost::lexical_cast<std::string>(port));
    boost::asio::ip::tcp::resolver resolver(io_service);
    start_connect(resolver.resolve(query));

    boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));
}

void Connector::start_connect(boost::asio::ip::basic_resolver<boost::asio::ip::tcp>::iterator iterator)
{
    boost::asio::async_connect(connection->get_socket(), iterator,
                               boost::bind(&Connector::handle_connect, this, boost::asio::placeholders::error));
}

void Connector::handle_connect(const boost::system::error_code& error){
    if(!error){
        connection_handler->add_connection(connection, EVENTS::CONNECTION_OPENED);
    }
    else
        std::cerr << error.message() << std::endl;
}
