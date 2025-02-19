cmake_minimum_required(VERSION 3.22)
project(FinalProject_OS)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add coverage flags for Debug builds with Clang
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Enabling code coverage flags for Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping -g -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-instr-generate")
endif()

# Include directories
include_directories(.)

# Default build type set to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add the source files for the project
add_executable(main
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

# Do not use -fprofile-instr-generate in target_link_libraries
# It should only be in CMAKE_CXX_FLAGS and CMAKE_EXE_LINKER_FLAGS.
