file(GLOB_RECURSE BOOST.ASIO_WRAPPER_SRC ${CMAKE_SOURCE_DIR} *.cpp)

# Create libboost.asio-wrapper and target alias
add_library(boost.asio-wrapper ${BOOST.ASIO_WRAPPER_SRC})
add_custom_target(libboost.asio-wrapper.a DEPENDS boost.asio-wrapper)

find_package(Boost COMPONENTS thread system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
find_package(Threads REQUIRED)
target_link_libraries(boost.asio-wrapper ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
