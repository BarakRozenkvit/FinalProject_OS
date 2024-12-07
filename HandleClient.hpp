//
// Created by Barak Rozenkvit on 04/12/2024.
//
#ifndef FINALPROJECT_OS_HANDLECLIENT_HPP
#define FINALPROJECT_OS_HANDLECLIENT_HPP
#include "Algo.hpp"
#include "MainGraph.hpp"
#include <iostream>

using namespace std;

namespace ClientHandler{

    void handle(MainGraph* graph, string command);
    string inputHandler();

}


#endif //FINALPROJECT_OS_HANDLECLIENT_HPP
