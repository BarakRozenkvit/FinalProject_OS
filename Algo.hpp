#include <queue>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include "Graph.hpp"
#pragma once

namespace MSTAlgo {
    /*
     * Source:  
     */
    pair<int,Graph> Prim(int fd, Graph g);
    /*
     * Source:  
     */
    pair<int,Graph> Kruskal(int fd, Graph g);
}

namespace DistanceAlgo {
    /*
     * Source:  
     */
    pair<int,Graph> FloyedWarshall(int fd, Graph g);
}

namespace GraphAlgo {
    /*
     * Get total weight of all the tree  
     */
    pair<int,Graph> getTotalWeight(int fd, Graph g);

    /*
    * Get the longest distance between to vetices in tree
    */
    pair<int,Graph> longestDistance(int fd, Graph g);

    /*
     * Get the average distance between to vetices in tree
     */
    pair<int,Graph> averageDistance(int fd, Graph g);

    /*
     * Get the shortest distance between to vetices in tree
     */
    pair<int,Graph> shortestDistance(int fd, Graph g);
}

namespace Util{
    
    /*
    * send to fd the messege
    */
    void outputHandler(string message, int fd);
}