cmake_minimum_required(VERSION 2.8)

project(util_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")

add_executable(util_test
			testingsetup.cpp
			test_for_test.cpp
			test_io.cpp)

target_link_libraries(util_test c rt curl pthread  boost_unit_test_framework boost_program_options boost_system )

