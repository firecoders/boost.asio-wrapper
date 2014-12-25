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

#ifndef WRAPPER_NETWORK_UTILS_CONNECTOR_GUARD
#define WRAPPER_NETWORK_UTILS_CONNECTOR_GUARD

#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include "wrapper/network/Connection.h"
#include "wrapper/network/events.h"

namespace wrapper
{
    namespace network
    {
        class Connection_handler;

        class Connector
        {
            public:

                Connector(boost::asio::io_service& io_service, Connection_handler* connection_handler, std::string ip, int port);

            private:

                void start_connect(boost::asio::ip::basic_resolver<boost::asio::ip::tcp>::iterator iterator);
                void handle_connect(const boost::system::error_code& error);

                boost::shared_ptr<wrapper::network::Connection> connection;
                Connection_handler* connection_handler;
                std::string ip;
                int port;
        };

    }
}

#endif /* WRAPPER_NETWORK_UTILS_CONNECTOR_GUARD */
