cmake_minimum_required(VERSION 2.8)

project(threadControl)
#set (threadControl_VERSION_MAJOR 1)
#set (threadControl_VERSION_MINOR 0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_BUILD_TYPE "Debug")


add_executable(threadControl
	main.cpp
	sync.cpp
	task_check.cpp)

target_link_libraries(threadControl pthread boost_system boost_thread)
