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

#ifndef WRAPPER_NETWORK_TYPES_DICT_GUARD
#define WRAPPER_NETWORK_TYPES_DICT_GUARD

#include "engine/types/Dict.hpp"

namespace wrapper
{
    namespace network
    {
        class Connection_handler;
        enum class EVENTS;
    } /* namespace wrapper */
} /* namespace network */

namespace engine
{
    namespace types
    {
        template <>
        struct Dict_types <wrapper::network::EVENTS>
        {
            static constexpr const char* type_string = "wrapper::network::EVENTS";
            static bool operator_equals(const Dynamic_union<Dict_types>& lhs, const Dynamic_union<Dict_types>& rhs)
            {
                return lhs.get<wrapper::network::EVENTS>() == rhs.get<wrapper::network::EVENTS>();
            }
            static bool operator_less(const Dynamic_union<Dict_types>& lhs, const Dynamic_union<Dict_types>& rhs)
            {
                return lhs.get<wrapper::network::EVENTS>() < rhs.get<wrapper::network::EVENTS>();
            }
        };

        template <>
        struct Dict_types <boost::shared_ptr<wrapper::network::Connection>>
        {
            static constexpr const char* type_string = "boost::shared_ptr<wrapper::network::Connection>";
            static bool operator_equals(const Dynamic_union<Dict_types>& lhs, const Dynamic_union<Dict_types>& rhs)
            {
                return lhs.get<boost::shared_ptr<wrapper::network::Connection>>() == rhs.get<boost::shared_ptr<wrapper::network::Connection>>();
            }
            static bool operator_less(const Dynamic_union<Dict_types>& lhs, const Dynamic_union<Dict_types>& rhs)
            {
                return lhs.get<boost::shared_ptr<wrapper::network::Connection>>() < rhs.get<boost::shared_ptr<wrapper::network::Connection>>();
            }
        };

        template <>
        struct Dict_types <wrapper::network::Connection_handler*>
        {
            static constexpr const char* type_string = "wrapper::network::Connection_handler*";
            static bool operator_equals(const Dynamic_union<Dict_types>& lhs, const Dynamic_union<Dict_types>& rhs)
            {
                return lhs.get<wrapper::network::Connection_handler*>() == rhs.get<wrapper::network::Connection_handler*>();
            }
            static bool operator_less(const Dynamic_union<Dict_types>& lhs, const Dynamic_union<Dict_types>& rhs)
            {
                return lhs.get<wrapper::network::Connection_handler*>() < rhs.get<wrapper::network::Connection_handler*>();
            }
        };
    } /* namespace types */
} /* namespace engine */

#endif /* WRAPPER_NETWORK_TYPES_DICT_GUARD */
