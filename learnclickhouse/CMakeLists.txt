cmake_minimum_required(VERSION 2.8)

project(ssl_share)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_BUILD_TYPE "Debug")
include_directories(./include)
add_subdirectory(clickhouse-cpp)
include_directories(clickhouse-cpp)
include_directories(clickhouse-cpp/contrib)

set(basic_src  src/main.cpp)
set(basic_link_lib curl pthread mysqlclient event boost_system boost_program_options boost_filesystem glog ssl crypto clickhouse-cpp-lib)


add_executable(clickhouselearn ${basic_src})
target_link_libraries(clickhouselearn ${basic_link_lib} gmp)

option (DEBUG_COUT "debug cout" OFF)


