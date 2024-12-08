//
// Created by Barak Rozenkvit on 04/12/2024.
//
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <condition_variable>
#include <mutex>
#include <sstream>
#include <pthread.h>
#include <vector>
#include "Proactor.hpp"
#include "Handlers.hpp"
#include "Algo.hpp"
#include "MainGraph.hpp"
#pragma once

using namespace std;

namespace ClientHandler{

    int handleGraph(int fd);

    void* handleClient(int fd);

    string inputHandler(string message,int fd);

    void outputHandler(string message, int fd);

    void* handleConnection(int fd);

}
