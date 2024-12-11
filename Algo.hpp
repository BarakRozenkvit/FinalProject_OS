#include "Tree.hpp"
#include "Graph.hpp"
#include "MainGraph.hpp"
#include <string>
#include <queue>
#pragma once

namespace MSTAlgo{

    Tree Boruvka(Graph& g);

    Tree Prim(Graph* g);

    Tree Kruskal(Graph* g);

    Tree Tarjan(Graph& g);

    Tree integerMST(Graph& g);

    class FactoryAlgo{

    public:
        static Tree applyAlgo(Graph* algo, std::string algoName);

    };
}

namespace DistanceAlgo{

    Graph FloyedMarshal(Graph g);
}

namespace Util{

}