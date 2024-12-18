#include <vector>
#include <iostream>
#include <map>
#include "Singletone.hpp"
#pragma once

using namespace std;

class Graph{

    /*
     * Graph class of undirected graph
     */
    vector<vector<int>> _graph;
    int _edgeCounter;

public:

    /*
     * Map of users and their graphs
     */
    static map<int,Graph> users_graphs;

    Graph() =default;
    
    /*
     * Copy constructor of graph
     */
    Graph(const Graph &g);
    
    ~Graph() = default;

    /*
     * Create New Graph and set _edges counter to zero
     */
    void newGraph(int n);

    /*
     * Add edge to graph, only if vertex and weight is allowed, add to graph counter
     */
    void addEdge(int v, int u, int w);

    /*
     * Remove edge from graph, set to zero and lower graph counter
     */
    void removeEdge(int v, int u);

    /*
    * Get number of vertices
    */
    int vertexNum();

    /*
     * Get number of edge
     */
    int edgeNum();

    /*
     * Get weight of edge
     */
    int at(int v, int u);

    /*
     * Get Deep copy of this graph
     */
    Graph getGraph() const;
};
