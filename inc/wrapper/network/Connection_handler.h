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

#ifndef WRAPPER_NETWORK_CONNECTION_HANDLER_GUARD
#define WRAPPER_NETWORK_CONNECTION_HANDLER_GUARD

#include <string>
#include <mutex>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include "wrapper/network/Connection.h"
#include "wrapper/network/utils/Acceptor.h"
#include "wrapper/network/utils/Connector.h"

#include "engine/events/interfaces/Receiver.h"
#include "wrapper/network/types/Dict.hpp"

namespace wrapper
{
    namespace network
    {
        class Connection_handler
        {
                friend class wrapper::network::Acceptor;
                friend class wrapper::network::Connector;

            public:

                Connection_handler(std::shared_ptr<engine::events::Receiver<std::shared_ptr<engine::types::Dict>>> receiver);

                void connect_to(std::string ip, int port);

                void accept_on(int port);
                void stop_accept_on(int port);
                bool contains_acceptor(int port);

                void remove_connection(wrapper::network::Connection_identifier& identifier);

                void handle_event(wrapper::network::EVENTS event, boost::shared_ptr<wrapper::network::Connection> activator, std::string message = "");

                boost::shared_ptr<const std::map<wrapper::network::Connection_identifier, boost::shared_ptr<wrapper::network::Connection> > > get_connections();

                bool contains(boost::shared_ptr<wrapper::network::Connection>& connection);
                bool contains(wrapper::network::Connection_identifier& identifier);

                void start();

                void wait_for_close();

                void close();

            private:

                void add_connection(boost::shared_ptr<wrapper::network::Connection>& connection, wrapper::network::EVENTS open_event);

                std::mutex join;

                boost::shared_mutex connections_mutex, acceptors_mutex;

                std::shared_ptr<engine::events::Receiver<std::shared_ptr<engine::types::Dict>>> receiver;

                boost::asio::io_service io_service;
                boost::shared_ptr<std::map<wrapper::network::Connection_identifier, boost::shared_ptr<wrapper::network::Connection> > > connections;
                std::map<int, boost::shared_ptr<wrapper::network::Acceptor> > acceptors;
        };
    }
}



#endif /* WRAPPER_NETWORK_CONNECTION_HANDLER_GUARD */
