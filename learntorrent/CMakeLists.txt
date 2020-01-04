cmake_minimum_required(VERSION 2.8)

project(learn_torrent)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")
include_directories(./include)

set(basic_src  src/tracker.cpp
               src/peer.cpp
               src/dht.cpp
               src/transfer.cpp 
               src/main.cpp)

set(basic_link_lib pthread boost_context torrent-rasterbar glog ssl crypto)

add_executable(torrentlearn ${basic_src})
target_link_libraries(torrentlearn ${basic_link_lib})

option (DEBUG_COUT "debug cout" OFF)


