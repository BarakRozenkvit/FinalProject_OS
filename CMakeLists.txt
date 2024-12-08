cmake_minimum_required(VERSION 3.22)
project(FinalProject_OS)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(FinalProject_OS
        Graph.cpp
        Graph.hpp
        Server.cpp
        MainGraph.cpp
        MainGraph.hpp
        Algo.cpp
        Algo.hpp
        Tree.cpp
        Tree.hpp
        ClientHandler.cpp
        ClientHandler.hpp
        Proactor.cpp
        Proactor.hpp
        Reactor.cpp
        Reactor.hpp
        Handlers.hpp
        Handlers.cpp
        )
