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

    pair<int,Graph> Prim(int fd, Graph g);

    pair<int,Graph> Kruskal(int fd, Graph g);
}

namespace DistanceAlgo {

    pair<int,Graph> FloyedWarshall(int fd, Graph g);
}

namespace GraphAlgo {
    
    pair<int,Graph> getTotalWeight(int fd, Graph g);

    pair<int,Graph> longestDistance(int fd, Graph g);

    pair<int,Graph> averageDistance(int fd, Graph g);

    pair<int,Graph> shortestDistance(int fd, Graph g);
}

namespace Util{
    
    void outputHandler(string message, int fd);
}