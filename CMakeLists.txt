cmake_minimum_required(VERSION 2.8.8)

project(scone)

find_package(Boost REQUIRED COMPONENTS)

include_directories(${Boost_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
add_subdirectory(scone/core)
