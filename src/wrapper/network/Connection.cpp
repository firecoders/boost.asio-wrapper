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

#include "wrapper/network/Connection.h"
#include "wrapper/network/Connection_handler.h"

using namespace wrapper::network;

Connection::Connection(boost::asio::io_service& io_service, Connection_handler& connection_handler):
        socket(io_service),
        state(CONNECTION_STATE::INITIALISED),
        identifier{"0.0.0.0", 0},
        connection_handler(connection_handler)
{
}

Connection_identifier& Connection::get_identifier()
{
    update_identifier();
    return identifier;
}

void Connection::update_identifier()
{
    identifier.ip = socket.local_endpoint().address().to_string();
    identifier.port = socket.local_endpoint().port();
}

void Connection::start()
{
    if(state == CONNECTION_STATE::INITIALISED)
        state = CONNECTION_STATE::OPENED;
}

void Connection::stop()
{
    state = CONNECTION_STATE::CLOSING;
    close();
}

void Connection::close()
{
    boost::unique_lock<boost::shared_mutex> lock(close_mutex);

    if(state == CONNECTION_STATE::CLOSING && message_chunks_empty() && read_queue_empty())
    {
        state = CONNECTION_STATE::CLOSED;
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket.close();
        connection_handler.handle_event(EVENTS::CONNECTION_CLOSED, shared_from_this());
    }
}

boost::asio::ip::tcp::socket& Connection::get_socket()
{
    return socket;
}

void Connection::write(std::string message)
{
    if(state == CONNECTION_STATE::OPENED)
    {
        boost::unique_lock<boost::shared_mutex> lock(message_push_mutex);

        push_message(message);
    }
}

void Connection::write(std::vector<std::string> messages)
{
    if(state == CONNECTION_STATE::OPENED)
    {
        boost::unique_lock<boost::shared_mutex> lock(message_push_mutex);

        for(std::vector<std::string>::iterator iterator = messages.begin(); iterator != messages.end(); ++iterator) {
            push_message(*iterator);
        }

    }
}

void Connection::push_message(std::string message)
{
    boost::unique_lock<boost::shared_mutex> lock(message_size_mutex);

    if (message_chunks.size() > 0)
        message_chunks.push(message);
    else if(message_chunks.empty())
    {
        message_chunks.push(message);

        boost::thread t(boost::bind(&Connection::write_queue, shared_from_this()));
        t.detach();
    }
}

void Connection::write_queue()
{
    boost::unique_lock<boost::shared_mutex> lock(write_mutex);

    while(!message_chunks_empty())
    {
        const std::string& message = message_chunks.front();
        boost::system::error_code error;
        boost::asio::write(socket, boost::asio::buffer(message.c_str(), message.size()), error);

        handle_error(error);
        connection_handler.handle_event(EVENTS::MESSAGE_SENDED, shared_from_this(), message);

        message_chunks.pop();
    }

    if(message_chunks_empty())
        close();
}

bool Connection::message_chunks_empty()
{
    boost::unique_lock<boost::shared_mutex> lock(message_size_mutex);

    return message_chunks.empty();
}

void Connection::read(const unsigned int size)
{
    if(state == CONNECTION_STATE::OPENED)
    {
        boost::unique_lock<boost::shared_mutex> lock(read_push_mutex);

        push_read([this, size] (boost::system::error_code& error) -> std::string {
            boost::asio::streambuf buf;
            boost::asio::read(socket, buf, boost::asio::transfer_exactly(size), error);

            std::ostringstream oss;
            oss << &buf;
            std::string message = oss.str();

            return message;
        });
    }
}

void Connection::read_until(const char c)
{
    if(state == CONNECTION_STATE::OPENED)
    {
        boost::unique_lock<boost::shared_mutex> lock(read_push_mutex);

        push_read([this, c] (boost::system::error_code& error) -> std::string {
            boost::asio::streambuf buf;
            boost::asio::read_until(socket, buf, c, error);

            std::ostringstream oss;
            oss << &buf;
            std::string message = oss.str();

            return message;
        });
    }
}

void Connection::read_until(const std::string s)
{
    if(state == CONNECTION_STATE::OPENED)
    {
        boost::unique_lock<boost::shared_mutex> lock(read_push_mutex);

        push_read([this, s] (boost::system::error_code& error) -> std::string {
            boost::asio::streambuf buf;
            boost::asio::read_until(socket, buf, s, error);

            std::ostringstream oss;
            oss << &buf;
            std::string message = oss.str();

            return message;
        });
    }
}

void Connection::read_until(const boost::regex expression)
{
    if(state == CONNECTION_STATE::OPENED)
    {
        boost::unique_lock<boost::shared_mutex> lock(read_push_mutex);

        push_read([this, expression] (boost::system::error_code& error) -> std::string {
            boost::asio::streambuf buf;
            boost::asio::read_until(socket, buf, expression, error);

            std::ostringstream oss;
            oss << &buf;
            std::string message = oss.str();

            return message;
        });
    }
}


void Connection::push_read(boost::function<std::string(boost::system::error_code)> function)
{
    boost::unique_lock<boost::shared_mutex> lock(read_size_mutex);

    if (read_queue.size() > 0)
        read_queue.push(function);
    else if(read_queue.empty())
    {
        read_queue.push(function);

        boost::thread t(boost::bind(&Connection::read_loop, shared_from_this()));
        t.detach();
    }
}

void Connection::read_loop()
{
    boost::unique_lock<boost::shared_mutex> lock(read_mutex);

    while(!read_queue_empty())
    {
        const boost::function<std::string(boost::system::error_code)>& function = read_queue.front();
        boost::system::error_code error;
        std::string message = function(error);

        handle_error(error);
        connection_handler.handle_event(EVENTS::MESSAGE_RECEIVED, shared_from_this(), message);

        read_queue.pop();
    }

    if(read_queue_empty())
        close();

}

bool Connection::read_queue_empty()
{
    boost::unique_lock<boost::shared_mutex> lock(read_size_mutex);

    return read_queue.empty();
}

void Connection::handle_error(const boost::system::error_code& error)
{
    if(error == boost::asio::error::eof){
        connection_handler.handle_event(EVENTS::CONNECTION_WAS_CLOSED, shared_from_this());
        close();
    }
    else if(error){
        std::cerr << error.message() << std::endl;
    }
}
