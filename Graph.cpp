#include "Graph.hpp"
#include <vector>

using namespace std;

void Graph::newGraph(int n) {
    _graph = vector<vector<int>>(n, vector<int>(n,0));
    _edgeCounter = 0;
}

void Graph::addEdge(int v, int u, int w) {
    if (v >= vertexNum() || u >= vertexNum() || v < 0 || u < 0){
        throw invalid_argument("index out of bounds!");
    }
    if (w < 0){
        throw invalid_argument("negative weight not allowed!");
    }
    if (_graph[v][u] > 0 && w > 0){
        _edgeCounter++;
    }
    _graph[v][u] = w;
    _graph[u][v] = w;
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

Graph::Graph(Graph const &g) {
    _graph = g._graph;
    _edgeCounter = g._edgeCounter;
}