//
// Created by Barak Rozenkvit on 04/12/2024.
//
#ifndef FINALPROJECT_OS_CLIENTHANDLER_HPP
#define FINALPROJECT_OS_CLIENTHANDLER_HPP
#include "Algo.hpp"
#include "MainGraph.hpp"
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

vector<pthread_t> handlers = {};

namespace ClientHandler{

    void* handleClient(MainGraph* graph, int fd);

    string inputHandler(string message,int fd);

    void* handleConnection(MainGraph* graph, int fd);

}


#endif //FINALPROJECT_OS_CLIENTHANDLER_HPP
