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
#include "Pipeline.hpp"
#include "LeaderFollower.hpp"
#include "Graph.hpp"
#pragma once

using namespace std;

extern std::vector<std::pair<pthread_t,void*>> handlers; // Declaration
extern pthread_cond_t condHandler;
extern pthread_mutex_t mutexHandler;

namespace ClientHandler{
    /*
     * Function that gets fd client and handle the graph requests 
     */
    int handleGraph(int fd);

    /*
    * Function the gets fd client had while handling graph is true keep going
    */
    void* handleClient(int fd);

    /**
     * get messege and fd, send messege and receive response and return it
     */
    string inputHandler(string message,int fd);

    /**
     * send messege to fd
     */
    void outputHandler(string message, int fd);

    /**
     * get fd of listener and accept new connections and add to threads list
     */
    void* handleConnection(int fd);

    /*
    * function to monitor all the handlers threads and if one is down, kill it so it wont be zombie and free data
    */
    void* monitorHandlers(int);

    /*
    * function to start thread of monitor handlers
    */
    void startMonitorHandlers();

    /*
    * function to kill all threads in threads list and free args
    */
    void killHandlers();
}
