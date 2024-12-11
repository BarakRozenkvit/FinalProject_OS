#include <queue>
#include <sstream>
#include <string>

#include "Graph.hpp"
#include "MainGraph.hpp"
#include "Tree.hpp"
#pragma once

namespace MSTAlgo {

Tree Boruvka(Graph& g);

Tree Prim(Graph g);

Tree Kruskal(Graph* g);

Tree Tarjan(Graph& g);

Tree integerMST(Graph& g);

class FactoryAlgo {
   public:
    static std::pair<Tree, std::string> applyAlgoWithDetails(Graph* graph, const std::string& algoName);
};
}  // namespace MSTAlgo

namespace DistanceAlgo {

Graph FloyedWarshall(Graph g);
}

namespace Util {

}