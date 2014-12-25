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

#include "wrapper/network/Connection_handler.h"

using namespace wrapper::network;

Connection_handler::Connection_handler(std::shared_ptr<engine::events::Receiver<std::shared_ptr<engine::types::Dict>>> receiver):
        receiver(receiver), connections(new std::map<Connection_identifier, boost::shared_ptr<Connection> >)
{
    join.lock();
}

void Connection_handler::connect_to(std::string ip, int port){
    Connection_identifier identifier{ip, port};

    if(!contains(identifier))
        new Connector(io_service, this, ip, port);
}

void Connection_handler::accept_on(int port){
    if(!contains_acceptor(port)){
        boost::unique_lock<boost::shared_mutex> lock(acceptors_mutex);

        boost::shared_ptr<Acceptor> acceptor = boost::shared_ptr<Acceptor>(new Acceptor(io_service, this, port));
        acceptors.insert(std::pair<int, boost::shared_ptr<Acceptor> >(port, acceptor));
    }
}

void Connection_handler::stop_accept_on(int port){
    if(contains_acceptor(port)){
        boost::unique_lock<boost::shared_mutex> lock(acceptors_mutex);

        acceptors[port]->close();
        acceptors.erase(port);
    }
}

bool Connection_handler::contains_acceptor(int port)
{
    boost::shared_lock<boost::shared_mutex> lock(acceptors_mutex);

    return acceptors.find(port) != acceptors.end();
}

void Connection_handler::remove_connection(Connection_identifier& identifier)
{
    if(contains(identifier)){
        (*connections)[identifier]->stop();

        boost::unique_lock<boost::shared_mutex> lock(connections_mutex);
        connections->erase(identifier);
    }
}

void Connection_handler::handle_event(wrapper::network::EVENTS event, boost::shared_ptr<wrapper::network::Connection> activator,
                                      std::string message)
{
    std::shared_ptr < engine::types::Dict > dict = std::make_shared < engine::types::Dict > ();

    dict->insert({std::string("event"), event});
    dict->insert({std::string("message"), message});
    dict->insert({std::string("activator"), activator});
    dict->insert({std::string("connection_handler"), this});

    receiver->receive(dict);
}

boost::shared_ptr<const std::map<Connection_identifier, boost::shared_ptr<Connection> > > Connection_handler::get_connections()
{
    boost::shared_lock<boost::shared_mutex> lock(connections_mutex);

    return connections;
}

bool Connection_handler::contains(Connection_identifier& identifier)
{
    boost::shared_lock<boost::shared_mutex> lock(connections_mutex);

    return connections->find(identifier) != connections->end();
}

bool Connection_handler::contains(boost::shared_ptr<Connection>& connection)
{
    return contains(connection->get_identifier());
}

void Connection_handler::start()
{
    join.try_lock();
}

void Connection_handler::wait_for_close()
{
    join.lock();
}

void Connection_handler::close()
{
    boost::unique_lock<boost::shared_mutex> connections_lock(connections_mutex);
    boost::unique_lock<boost::shared_mutex> acceptors_lock(acceptors_mutex);

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

    join.unlock();
}

void Connection_handler::add_connection(boost::shared_ptr<Connection>& connection, EVENTS open_event)
{
    {
        boost::unique_lock<boost::shared_mutex> lock(connections_mutex);

        connections->insert(std::pair<Connection_identifier, boost::shared_ptr<Connection> >(connection->get_identifier(), connection));
    }

    connection->start();
    handle_event(open_event, connection);
}
