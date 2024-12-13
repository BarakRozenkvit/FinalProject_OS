#include "Algo.hpp"

#include <algorithm>  // for std::min

Graph MSTAlgo::Prim(Graph graph) {
    cout << "Starting Prim" << endl;
    int v = graph.vertexNum();
    // vector to store the parent of vertex
    vector<int> parent(v);

    // vector holds the weight/ cost of the MST
    vector<int> key(v);

    // vector to represent the set of
    // vertices included in MST
    vector<bool> vis(v);

    priority_queue<pair<int, int>,
                   vector<pair<int, int>>,
                   greater<pair<int, int>>>
        pq;

    // Initialize all key vector as INFINITE
    // and vis vector as false
    for (int i = 0; i < v; i++) {
        key[i] = INT_MAX;
        vis[i] = false;
    }

    // Always include the first vertex in MST.
    // Make key 0 so that this vertex is
    // picked as the first vertex.
    key[0] = 0;

    // First node is always the root of MST
    parent[0] = -1;

    // Push the source vertex to the min-heap
    pq.push({0, 0});

    while (!pq.empty()) {
        int node = pq.top().second;
        pq.pop();
        vis[node] = true;
        for (int i = 0; i < v; i++) {
            // If the vertex is not visited
            // and the edge weight of neighbouring
            // vertex is less than key value of
            // neighbouring vertex then update it.
            if (!vis[i] && graph.at(node, i) != 0 && graph.at(node, i) < key[i]) {
                pq.push({graph.at(node, i), i});
                key[i] = graph.at(node, i);
                parent[i] = node;
            }
        }
    }

    Graph tree;
    tree.newGraph(graph.vertexNum());
    // Print the edges and their
    // weights in the MST
    for (int i = 1; i < v; i++) {
        tree.addEdge(parent[i], i, graph.at(i, parent[i]));
        tree.addEdge(i, parent[i], graph.at(i, parent[i]));
    }
    cout << "Stopped Prim" << endl;
    return tree;
}

Graph MSTAlgo::Kruskal(Graph graph) {
    cout << "Starting Kruskal" << endl;
    int V = graph.vertexNum();
    vector<pair<int, pair<int, int>>> edges;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (graph.at(i, j) != 0) {
                edges.push_back({graph.at(i, j), {i, j}});
            }
        }
    }
    sort(edges.begin(), edges.end());

    Graph tree;
    tree.newGraph(graph.vertexNum());
    int* parent = new int[V];
    for (int i = 0; i < V; i++) {
        parent[i] = i;
    }
    int i = 0, e = 0;
    while (e < V - 1 && i < edges.size()) {
        int w = edges[i].first;
        int u = edges[i].second.first;
        int v = edges[i].second.second;
        i++;
        int x = u;
        int y = v;
        while (parent[x] != x) {
            x = parent[x];
        }
        while (parent[y] != y) {
            y = parent[y];
        }
        if (x != y) {
            tree.addEdge(u, v, w);
            tree.addEdge(v, u, w);
            e++;
            parent[x] = y;
        }
    }
    cout << "Stopped Kruskal" << endl;
    return tree;
}

Graph DistanceAlgo::FloyedWarshall(Graph g) {
    /**
     * 1. D(0) ← W
2. 3. 4. 5. for k ← 1 to n
do for i ← 1 to n
do for j ← 1 to n
do dij(k) ← min (dij(k-1), dik(k-1) + dkj(k-1))
6. return D(n)
     */
    cout << "Starting Floyed" << endl;
    for (int k = 0; k < g.vertexNum(); k++) {
        for (int i = 0; i < g.vertexNum(); i++) {
            for (int j = 0; j < g.vertexNum(); j++) {
                if (g.at(i, j) && g.at(i, k) + g.at(k, j)) {
                    g.addEdge(i, j, std::min(g.at(i, j), g.at(i, k) + g.at(k, j)));
                }
            }
        }
    }
    cout << "Stopped Floyed" << endl;
    return g;
}

Graph GraphAlgo::getTotalWeight(Graph graph) {
    int weight = 0;
    for(int i=0;i<graph.vertexNum();i++){
        for(int j=0;j<i;j++){
            weight += graph.at(i,j);
        }
    }
    cout << "getTotalWeight: " << weight << endl;
    return graph;
}

Graph GraphAlgo::longestDistance(Graph graph) {
    int d=0;
    for(int i=0;i<graph.vertexNum();i++){
        for(int j=0;j<i;j++){
            if (graph.at(i,j) > d){
                d= graph.at(i,j);
            }
        }
    }
    cout << "longestDistance: " << d << endl;
    return graph;
}

Graph GraphAlgo::shortestDistance(Graph graph) {
    int d=INT_MAX;
    for(int i=0;i<graph.vertexNum();i++){
        for(int j=0;j<i;j++){
            if (graph.at(i,j) > 0 && graph.at(i,j) < d){
                d= graph.at(i,j);
            }
        }
    }
    cout << "shortestDistance: " << d << endl;
    return graph;

}

Graph GraphAlgo::averageDistance(Graph graph) {
    double totalDistance = 0; // Total sum of distances
    int count = 0;           // Count of unique pairs

    // Iterate over all unique pairs (i < j)
    for (int i = 0; i < graph.vertexNum(); i++) {
        for (int j = i + 1; j < graph.vertexNum(); j++) {
            if (graph.at(i, j) != INT_MAX) { // Include valid shortest paths
                totalDistance += graph.at(i, j);
                count++;
            }
        }
    }
    cout << "shortestDistance: " << totalDistance/count << endl;
    // Compute and return average distance
    return graph;
}