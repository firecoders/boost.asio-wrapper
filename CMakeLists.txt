# boost.asio-wrapper, A C++ library, wrapping boost.asio, to be used for client or server applications
# Copyright (C) 2014 firecoders
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
# OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


project(BOOST.ASIO_WRAPPER)
cmake_minimum_required(VERSION 2.6)
include (ExternalProject)

# Output compiled files in root directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/..)

# Use C++11 standard
if(CMAKE_CXX_COMPILER MATCHES ".*clang")
    set(CMAKE_COMPILER_IS_CLANG)
endif(CMAKE_CXX_COMPILER MATCHES ".*clang")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)

# Store external dependencies in build/external/
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O1")

# Store external dependencies in build/external/
set(EXTERNALS_PREFIX ${PROJECT_BINARY_DIR}/external CACHE FILEPATH "Installation Path")
set(EXTERNALS_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNALS_PREFIX})

# Add the eventsystem
ExternalProject_Add ( EVENTSYSTEM
    PREFIX ${EXTERNALS_PREFIX}
    URL https://github.com/firecoders/eventsystem/archive/v0.2.2.tar.gz
    URL_HASH SHA256=35d4a3d6da47a67d501baa473997152e097efc67b8a1075db8098a5153a7d75b
    DOWNLOAD_NAME eventsystem-v0.2.2.tar.gz
    CMAKE_ARGS ${EXTERNALS_CMAKE_ARGS}
)

# Import libeventsystem.a
add_library ( libeventsystem STATIC IMPORTED )
set_target_properties ( libeventsystem PROPERTIES IMPORTED_LOCATION ${EXTERNALS_PREFIX}/lib/libeventsystem.a )
add_dependencies ( libeventsystem EVENTSYSTEM )

# Include headers from inc
include_directories(${CMAKE_SOURCE_DIR}/inc)

# Add a subdirectory
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

