#include "Graph.hpp"
#include <vector>

using namespace std;

void Graph::newGraph(int n) {
    _graph = vector<vector<int>>(n, vector<int>(n,0));
    _edgeCounter = 0;
}

void Graph::addEdge(int v, int u, int w) {
    _graph[v][u] = w;
    _graph[u][v] = w;
    _edgeCounter++;
}

void Graph::removeEdge(int v, int u) {
    _graph[v][u] = 0;
    _graph[u][v] = 0;
    _edgeCounter--;
}

int Graph::edgeNum() {
    return _edgeCounter;
}

int Graph::vertexNum() {
    return _graph.size();
}

int Graph::at(int v, int u) {
    return _graph[v][u];
}
