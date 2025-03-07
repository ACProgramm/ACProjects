cmake_minimum_required(VERSION 3.10)

project(MathClientServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(server src/main_server.cpp src/server.cpp)
target_link_libraries(server Boost::boost pthread)

add_executable(client src/main_client.cpp src/client.cpp)
target_link_libraries(client Boost::boost pthread)
