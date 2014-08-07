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

#include "Connection_handler.h"

using namespace wrapper::network;

Connection_handler::Connection_handler():
        connections(new std::map<Connection_identifier, boost::shared_ptr<Connection> >)
{
}

void Connection_handler::connect_to(std::string ip, int port){
    Connection_identifier identifier(ip, port);

    if(!contains(identifier))
        new Connector(io_service, this, ip, port);
}

void Connection_handler::accept_on(int port){
    if(!contains_acceptor(port)){
        boost::shared_ptr<Acceptor> acceptor = boost::shared_ptr<Acceptor>(new Acceptor(io_service, this, port));
        acceptors.insert(std::pair<int, boost::shared_ptr<Acceptor> >(port, acceptor));
    }
}

void Connection_handler::stop_accept_on(int port){
    if(contains_acceptor(port)){
        acceptors[port]->close();
        acceptors.erase(port);
    }
}

void Connection_handler::remove_connection(Connection_identifier& identifier)
{
    if(contains(identifier)){
        (*connections)[identifier]->stop();
    }
}

void Connection_handler::handle_event(wrapper::network::EVENTS event, std::string message)
{
}

boost::shared_ptr<std::map<Connection_identifier, boost::shared_ptr<Connection> > > Connection_handler::get_connections()
{
    return connections;
}


bool Connection_handler::contains(boost::shared_ptr<Connection>& connection)
{
    return contains(connection->get_identifier());
}

bool Connection_handler::contains(Connection_identifier& identifier)
{
    return connections->find(identifier) != connections->end();
}

void Connection_handler::close()
{
    io_service.stop();

    for (std::map<Connection_identifier, boost::shared_ptr<Connection> >::iterator it_p_connection = connections->begin();
            it_p_connection != connections->end();
            ++it_p_connection)
        (it_p_connection->second)->stop();
    connections->clear();

    for (std::map<int, boost::shared_ptr<Acceptor> >::iterator it_p_acceptor = acceptors.begin();
            it_p_acceptor != acceptors.end();
            ++it_p_acceptor)
        (it_p_acceptor->second)->close();
    acceptors.clear();
}

void Connection_handler::add_connection(boost::shared_ptr<Connection>& connection, EVENTS open_event)
{
    if(!contains(connection)){
        connections->insert(std::pair<Connection_identifier, boost::shared_ptr<Connection> >(connection->get_identifier(), connection));
        connection->start();
        handle_event(open_event);
    }
    else
        connection->stop();
}

bool Connection_handler::contains_acceptor(int port)
{
    return acceptors.find(port) != acceptors.end();
}
