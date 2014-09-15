project(BOOST.ASIO_WRAPPER)
cmake_minimum_required(VERSION 3.0)

# Output compiled files in root directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/..)

# Use C++11 standard
if(CMAKE_CXX_COMPILER MATCHES ".*clang")
	set(CMAKE_COMPILER_IS_CLANG)
endif(CMAKE_CXX_COMPILER MATCHES ".*clang")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O1")

# Include headers from inc
include_directories(${CMAKE_SOURCE_DIR}/inc)

# Add a subdirectory
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

