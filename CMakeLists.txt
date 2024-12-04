cmake_minimum_required(VERSION 3.22)
project(FinalProject_OS)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(FinalProject_OS
        Graph.cpp
        Graph.hpp
        Guard.cpp
        Guard.hpp
        HandleClient.cpp
        HandleClient.hpp
        MSTAlgo.cpp
        MSTAlgo.hpp
        Server.cpp
        Tree.cpp
        Tree.hpp)
