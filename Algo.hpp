#include <queue>
#include <sstream>
#include <string>
#include "Graph.hpp"
#pragma once

namespace MSTAlgo {

    Graph Prim(Graph* g);

    Graph Kruskal(Graph* g);
}

namespace DistanceAlgo {

    Graph FloyedWarshall(Graph* g);
}

namespace GraphAlgo {
    
    Graph getTotalWeight(Graph* g);

    Graph longestDistance(Graph* g);

    Graph averageDistance(Graph* g);

    Graph shortestDistance(Graph* g);
}