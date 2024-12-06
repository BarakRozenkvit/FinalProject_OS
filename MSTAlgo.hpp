#include "Tree.hpp"
#include "Graph.hpp"
#include <string>

namespace MSTAlgo{

    Tree Boruvka(Graph g);

    Tree Prim(Graph g);

    Tree Kruskal(Graph g);

    Tree Tarjan(Graph g);

    Tree integerMST(Graph g);

    class FactoryAlgo{

    public:
        static Tree applyAlgo(string algo);

    };
}

namespace Util{

}