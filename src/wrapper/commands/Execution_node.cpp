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

#include "wrapper/commands/Execution_node.h"

using namespace wrapper::commands;

Execution_node::Execution_node(std::shared_ptr<Command> command, EXECUTION_TYPE type):
                                command(command), type(type)
{
}

void Execution_node::execute(Command_params& params)
{
    if(type == EXECUTION_TYPE::ASYNCHRONOUS)
    {
        boost::thread(boost::bind(&Execution_node::start_execute, this, params));
    }
    else if(type == EXECUTION_TYPE::SYNCHRONOUS)
    {
        start_execute(params);
    }
    else
    {
        boost::unique_lock<boost::shared_mutex> lock(unique_mutex);
        start_execute(params);
    }
}

bool Execution_node::match(Command_params& params)
{
    return true;
}

void Execution_node::set_command(std::shared_ptr<Command> command)
{
    this->command = command;
}

void Execution_node::set_execution_type(EXECUTION_TYPE type)
{
    this->type = type;
}

void Execution_node::start_execute(Command_params& params)
{
    if(command->match(params))
        command->execute(params);
}

