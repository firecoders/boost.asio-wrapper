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

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <string>
#include <deque>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "wrapper/network/events.h"

namespace wrapper
{
    namespace network
    {
        class Connection_handler;

        enum class CONNECTION_STATE
        {
                INITIALISED,
                OPENED,
                CLOSED
        };

        struct Connection_identifier
        {
            public:

                std::string ip;
                int port;

                Connection_identifier(std::string ip, int port):
                    ip(ip), port(port)
                {
                }

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

            private:

                std::string to_string(const std::string& s_ip, int i_port) const
                {
                    return s_ip + ":" + std::to_string(i_port);
                }
        };

        class Connection:
                public boost::enable_shared_from_this<Connection>
        {
            public:

                Connection(boost::asio::io_service& io_service, wrapper::network::Connection_handler& connection_handler);

                boost::asio::ip::tcp::socket& get_socket();

                wrapper::network::Connection_identifier& get_identifier();

                void start();
                void stop();
                void write(std::string message);

            private:

                void close();

                void update_identifier();

                void handle_error(const boost::system::error_code& error);

                void read_header();
                void read_body(int message_size);
                int get_message_size(boost::asio::streambuf& buf);
                void handle_read_header(const boost::system::error_code& error);
                void handle_read_body(const boost::system::error_code& error);

                void write_message(std::string message);
                void write_deque();
                std::string make_header(std::string& message);
                void handle_write_deque(const boost::system::error_code& error);

                boost::asio::ip::tcp::socket socket;
                boost::asio::io_service::strand strand;
                boost::asio::streambuf buf_header, buf_body;
                std::deque<std::string> deque_message;
                wrapper::network::CONNECTION_STATE state;
                wrapper::network::Connection_identifier identifier;
                wrapper::network::Connection_handler& connection_handler;
        };
    }
}



#endif /* CONNECTION_H_ */
