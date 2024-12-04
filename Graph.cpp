#include "Graph.hpp"
#include <vector>

using namespace std;

void Graph::NewGraph(int n) {
    _graph = vector<vector<int>>(n, vector<int>(n,0));
    _edgeCounter = 0;
}

void Graph::addEdge(int v, int u, int w) {
    _graph[v][u] = w;
    _edgeCounter++;
}

void Graph::removeEdge(int v, int u) {
    _graph[v][u] = 0;
    _edgeCounter--;
}

int Graph::edgeNum() {
    return _edgeCounter;
}

int Graph::vertexNum() {
    return _graph.size();
}


int main(){

    Graph graph = Graph();
    graph.NewGraph(2);
    const vector<vector<int>>& f = graph.getGraph();
    cout << f[1][0] <<endl;
    cout << &f << endl;

    return 0;

}
