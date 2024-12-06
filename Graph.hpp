#include <vector>
#include "Guard.hpp"
#include <iostream>
#pragma once

using namespace std;

class Graph: public Guard{
    
    vector<vector<int>> _graph;
    int _edgeCounter;

public:
    Graph() = default;
    ~Graph() = default;

    void newGraph(int n);

    void addEdge(int v, int u, int w);

    void removeEdge(int v, int u);

    int vertexNum();

    int edgeNum();

    int at(int v, int u);

    const vector<vector<int>>& getGraph() const{
        return _graph;
    }

};
