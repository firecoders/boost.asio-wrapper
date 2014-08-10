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

#include "Command_hub.h"

using namespace wrapper::commands;

Command_hub::Command_hub(std::function<bool(Command_params& params)> f_match):
        f_match(f_match)
{
    if(f_match == nullptr)
        use_default_match();
}

void Command_hub::execute(Command_params& params)
{
    for(std::vector<std::shared_ptr<Command>>::iterator it = commands.begin(); it != commands.end(); ++it)
    {
        if((*it)->match(params))
            (*it)->execute(params);
    }
}

bool Command_hub::match(Command_params& params)
{
    return f_match(params);
}

void Command_hub::add_command(std::shared_ptr<Command> command)
{
    commands.push_back(command);
}

void Command_hub::set_match(std::function<bool(Command_params& params)> new_f_match)
{
    f_match = new_f_match;

    if(f_match == nullptr)
        use_default_match();
}

bool Command_hub::default_match(Command_params& params)
{
    for(std::vector<std::shared_ptr<Command>>::iterator it = commands.begin(); it != commands.end(); ++it)
    {
        if((*it)->match(params))
            return true;
    }

    return false;
}

void Command_hub::use_default_match()
{
    f_match = std::bind(&Command_hub::default_match, this, std::placeholders::_1);
}
