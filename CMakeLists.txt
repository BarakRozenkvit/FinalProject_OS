cmake_minimum_required(VERSION 3.22)
project(FinalProject_OS)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

# Debug configuration
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -fno-omit-frame-pointer")

# Disable optimizations for better debugging
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

add_executable(FinalProject_OS
        Graph.cpp
        Graph.hpp
        Server.cpp
        Algo.cpp
        Algo.hpp
        ClientHandler.cpp
        ClientHandler.hpp
        Proactor.cpp
        Proactor.hpp
        Reactor.cpp
        Reactor.hpp
        ActiveObject.hpp
        ActiveObject.cpp
        Pipeline.hpp
        Pipeline.cpp
        LeaderFollower.cpp
        LeaderFollower.hpp
        Singletone.hpp
        )

# add_executable(Kruskal
#  Algo.hpp
#         Algo.cpp
#         Tree.hpp
#         Tree.cpp
#         MainGraph.hpp
#         MainGraph.cpp
#         Graph.hpp
#         Graph.cpp
# )