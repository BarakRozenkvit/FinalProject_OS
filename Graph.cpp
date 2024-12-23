#include "Graph.hpp"
#include <vector>

using namespace std;

map<int,Graph> Graph::users_graphs = map<int,Graph>();

void Graph::newGraph(int n) {
    _graph = vector<vector<int>>(n, vector<int>(n, 0));
    _edgeCounter = 0;
}

void Graph::addEdge(int v, int u, int w) {
    if (v >= vertexNum() || u >= vertexNum() || v < 0 || u < 0) {
        throw invalid_argument("index out of bounds!");
    }
    if (w < 0) {
        throw invalid_argument("negative weight not allowed!");
    }
    if (!_graph[v][u] && w > 0) {
        _edgeCounter++;
    } else if (_graph[v][u] > 0 && w == 0) {
        _edgeCounter--;
    }

    // Graph is undirected
    _graph[v][u] = w;
    _graph[u][v] = w;
}

void Graph::removeEdge(int v, int u) {
    _graph[v][u] = 0;
    _graph[u][v] = 0;
    _edgeCounter--;
}

int Graph::edgeNum() const{
    return _edgeCounter;
}

int Graph::vertexNum() const {
    return _graph.size();
}

int Graph::at(int v, int u) const {
    return _graph[v][u];
}

Graph::Graph(Graph const &g) {
    _graph = g._graph;
    _edgeCounter = g._edgeCounter;
}

Graph Graph::getGraph() const{
    return *this;
}

void Graph::DFS(int v, vector<bool>& visited) const {
    visited[v] = true;
    for(int i = 0; i < vertexNum(); i++) {
        if(_graph[v][i] && !visited[i]) {
            DFS(i, visited);
        }
    }
}

bool Graph::isConnected() const {
    if(vertexNum() == 0) return true;
    
    vector<bool> visited(vertexNum(), false);
    // Start DFS from vertex 0
    DFS(0, visited);
    
    // Check if all vertices were visited
    for(int i = 0; i < vertexNum(); i++) {
        if(!visited[i]) return false;
    }
    return true;
}