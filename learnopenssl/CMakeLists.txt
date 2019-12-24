cmake_minimum_required(VERSION 2.8)

project(ssl_share)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")
include_directories(./include)

set(basic_src  src/open_ecc.cpp
               src/main.cpp)



set(basic_link_lib curl pthread mysqlclient event boost_system boost_program_options boost_filesystem glog ssl crypto)


add_executable(ssllearn ${basic_src})
target_link_libraries(ssllearn ${basic_link_lib} gmp)

option (DEBUG_COUT "debug cout" OFF)


