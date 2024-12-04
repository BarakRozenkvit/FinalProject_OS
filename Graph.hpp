#include <vector>
#include "Guard.hpp"
#include <iostream>

using namespace std;

class Graph: public Guard{
    
    vector<vector<int>> _graph;
    int _edgeCounter;

public:
    Graph() = default;
    ~Graph() =default;

    void NewGraph(int n);

    void addEdge(int v, int u, int w);

    void removeEdge(int v, int u);

    int vertexNum();

    int edgeNum();

    const vector<vector<int>>& getGraph() const{
        return _graph;
    }

};
