cmake_minimum_required(VERSION 3.5)

project (fuzzy-k-means)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++11 -g")

#Bring headers
include_directories(include)

#Define sources
file(GLOB SOURCES "src/*.cpp" "point/*.cpp")

add_executable(fkm ${SOURCES})
