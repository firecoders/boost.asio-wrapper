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

#ifndef WRAPPER_NETWORK_CONNECTION_GUARD
#define WRAPPER_NETWORK_CONNECTION_GUARD

#include <string>
#include <vector>
#include <queue>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "wrapper/network/events.h"

namespace wrapper
{
    namespace network
    {
        class Acceptor;
        class Connector;

        class Connection_handler;

        enum class CONNECTION_STATE
        {
                INITIALISED,
                OPENED,
                CLOSING,
                CLOSED
        };

        struct Connection_identifier
        {
                std::string ip;
                int port;

                bool operator==(const Connection_identifier& other) const
                {
                    return to_string(ip, port) == to_string(other.ip, other.port);
                }

                bool operator!=(const Connection_identifier& other) const
                {
                    return to_string(ip, port) != to_string(other.ip, other.port);
                }

                bool operator<(const Connection_identifier& other) const
                {
                    return to_string(ip, port) < to_string(other.ip, other.port);
                }

                bool operator>(const Connection_identifier& other) const
                {
                    return to_string(ip, port) > to_string(other.ip, other.port);
                }

                std::string to_string(const std::string& s_ip, int i_port) const
                {
                    return s_ip + " : " + std::to_string(i_port);
                }
        };

        class Connection : public boost::enable_shared_from_this<Connection>
        {
                friend class Acceptor;
                friend class Connector;

            public:

                Connection(boost::asio::io_service& io_service, wrapper::network::Connection_handler& connection_handler);

                Connection_identifier& get_identifier();

                void start();
                void stop();

                void write(std::string message);
                void write(std::vector<std::string> messages);

                void read(const unsigned int size);
                void read_until(const char c);
                void read_until(const std::string s);
                void read_until(const boost::regex expression);

            private:

                boost::asio::ip::tcp::socket& get_socket();

                void update_identifier();

                void close();

                void push_message(std::string message);
                void write_queue();
                bool message_chunks_empty();

                void push_read(boost::function<std::string(boost::system::error_code)> function);
                void read_loop();
                bool read_queue_empty();

                void handle_error(const boost::system::error_code& error);

                boost::asio::ip::tcp::socket socket;
                boost::shared_mutex message_push_mutex, message_size_mutex, write_mutex;
                boost::shared_mutex close_mutex;
                boost::shared_mutex read_push_mutex, read_size_mutex, read_mutex;

                std::queue<std::string> message_chunks;
                std::queue<boost::function<std::string(boost::system::error_code)>> read_queue;

                CONNECTION_STATE state;
                Connection_identifier identifier;
                Connection_handler& connection_handler;
        };
    }
}

#endif /* WRAPPER_NETWORK_CONNECTION_GUARD */
