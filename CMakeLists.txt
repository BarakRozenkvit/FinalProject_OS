cmake_minimum_required(VERSION 3.22)
project(FinalProject_OS)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(FinalProject_OS
        Graph.cpp
        Graph.hpp
        Server.cpp
        MainGraph.cpp
        MainGraph.hpp
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