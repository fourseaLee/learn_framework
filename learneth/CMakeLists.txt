cmake_minimum_required(VERSION 2.8)

project(ssl_share)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")
include_directories(./include)

set(basic_src  src/main.cpp)



set(basic_link_lib curl pthread crypto++ ssl crypto)


add_executable(ethlearn ${basic_src})
target_link_libraries(ethlearn ${basic_link_lib})

option (DEBUG_COUT "debug cout" OFF)


