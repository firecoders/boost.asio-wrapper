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
        socket(io_service), strand(io_service),
        state(CONNECTION_STATE::INITIALISED),
        identifier("0.0.0.0", 0),
        connection_handler(connection_handler)
{
}

Connection_identifier& Connection::get_identifier()
{
    update_identifier();
    return identifier;
}

void Connection::start()
{
    if(state == CONNECTION_STATE::INITIALISED)
    {
        state = CONNECTION_STATE::OPENED;
        read_header();
    }
}

void Connection::stop()
{
    close();
    connection_handler.handle_event(EVENTS::CONNECTION_CLOSED, shared_from_this());
}

void Connection::write(std::string message)
{
    if(state == CONNECTION_STATE::OPENED)
        strand.post(boost::bind(&Connection::write_message,
                                shared_from_this(), message));
}

boost::asio::ip::tcp::socket& Connection::get_socket()
{
    return socket;
}

void Connection::close()
{
    state = CONNECTION_STATE::CLOSED;

    if(message_chunks.empty())
    {
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket.close();
    }
}

void Connection::update_identifier()
{
    identifier.ip = socket.local_endpoint().address().to_string();
    identifier.port = socket.local_endpoint().port();
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

void Connection::read_header()
{
    boost::asio::async_read(socket, buf_header, boost::asio::transfer_exactly(32),
                            boost::bind(&Connection::handle_read_header, shared_from_this(),
                                        boost::asio::placeholders::error));
}

void Connection::handle_read_header(const boost::system::error_code& error)
{
    if(!error){
        int message_size = get_message_size(buf_header);
        read_body(message_size);
    }
    else
        handle_error(error);
}

void Connection::read_body(int message_size){
    boost::asio::async_read(socket, buf_body, boost::asio::transfer_exactly(message_size),
                            boost::bind(&Connection::handle_read_body, shared_from_this(),
                                        boost::asio::placeholders::error));
}

void Connection::handle_read_body(const boost::system::error_code& error)
{
    if(!error){

        std::ostringstream ss;
        ss << &buf_body;
        std::string body = ss.str();

        connection_handler.handle_event(EVENTS::MESSAGE_RECEIVED, shared_from_this(), body);
        read_header();
    }
    else
        handle_error(error);
}

int Connection::get_message_size(boost::asio::streambuf& buf)
{
    std::ostringstream ss;
    ss << &buf;
    std::string header = ss.str();

    header = header.substr(0, header.find(" "));
    return boost::lexical_cast<int>(header);
}

void Connection::write_message(std::string message){
    message_chunks.push(make_header(message));
    message_chunks.push(message);

    // Returns immediately if the Connection is already sending
    if (message_chunks.size() > 2)
    {
        return;
    }

    write_queue();
}

void Connection::write_queue(){
    const std::string& message = message_chunks.front();
    boost::asio::async_write(socket, boost::asio::buffer(message.c_str(), message.size()),
                             strand.wrap(boost::bind(&Connection::handle_write_queue,
                                                     shared_from_this(),
                                                     boost::asio::placeholders::error)));
}

void Connection::handle_write_queue(const boost::system::error_code& error){
    message_chunks.pop();

    handle_error(error);

    // If 'message_chunks' is not empty, continue sending
    if(!message_chunks.empty())
    {
        write_queue();
    }

    if(state == CONNECTION_STATE::CLOSED)
    {
        close();
    }
}

std::string Connection::make_header(std::string& message){
    std::string header = boost::lexical_cast<std::string>(message.size());
    while(header.size() < 32)
        header.append(" ");
    return header;
}
