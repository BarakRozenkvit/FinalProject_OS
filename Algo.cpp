#include "Algo.hpp"

#include <algorithm> 

pair<int,Graph> MSTAlgo::Prim(int fd, Graph graph) {

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
    // Print the _edges and their
    // weights in the MST
    for (int i = 1; i < v; i++) {
        tree.addEdge(parent[i], i, graph.at(i, parent[i]));
        tree.addEdge(i, parent[i], graph.at(i, parent[i]));
    }
    // send the fd the result
    Util::outputHandler("Finished Prim\n",fd);
    return make_pair(fd,tree);
}

pair<int,Graph> MSTAlgo::Kruskal(int fd, Graph graph) {
    int V = graph.vertexNum();
    vector<pair<int, pair<int, int>>> _edges;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (graph.at(i, j) != 0) {
                _edges.push_back({graph.at(i, j), {i, j}});
            }
        }
    }
    sort(_edges.begin(), _edges.end());

    Graph tree;
    tree.newGraph(graph.vertexNum());
    int* parent = new int[V];
    for (int i = 0; i < V; i++) {
        parent[i] = i;
    }
    int i = 0, e = 0;
    while (e < V - 1 && i < _edges.size()) {
        int w = _edges[i].first;
        int u = _edges[i].second.first;
        int v = _edges[i].second.second;
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
    // send the fd the result
    Util::outputHandler("Finished Kruskal\n",fd);
    return make_pair(fd,tree);
}

pair<int,Graph> DistanceAlgo::FloydWarshall(int fd, Graph g) {
    /**
    *   1. D(0) ← W
        2. for k ← 1 to n
            3. do for i ← 1 to n
                4. do for j ← 1 to n
                    5. do dij(k) ← min (dij(k-1), dik(k-1) + dkj(k-1))
        6. return D(n)
     */
    for (int k = 0; k < g.vertexNum(); k++) {
        for (int i = 0; i < g.vertexNum(); i++) {
            for (int j = 0; j < g.vertexNum(); j++) {
                if (g.at(i, j) && g.at(i, k) + g.at(k, j)) {
                    g.addEdge(i, j, std::min(g.at(i, j), g.at(i, k) + g.at(k, j)));
                }
            }
        }
    }
    // send the fd the result
    Util::outputHandler("\nFinished Floyd Warshall\n",fd);
    return make_pair(fd,g);
}

pair<int,Graph> GraphAlgo::getTotalWeight(int fd, Graph graph) {
    int weight = 0;
    for(int i=0;i<graph.vertexNum();i++){
        for(int j=0;j<i;j++){
            weight += graph.at(i,j);
        }
    }
    // send the fd the result
    Util::outputHandler("getTotalWeight: " + to_string(weight),fd);
    return make_pair(fd,graph);
}

pair<int,Graph> GraphAlgo::longestDistance(int fd, Graph graph) {
    int d=0;
    for(int i=0;i<graph.vertexNum();i++){
        for(int j=0;j<i;j++){
            if (graph.at(i,j) > d){
                d= graph.at(i,j);
            }
        }
    }
    // send the fd the result
    Util::outputHandler("longestDistance: " + to_string(d) +"\n",fd);
    return make_pair(fd,graph);
}

pair<int,Graph> GraphAlgo::shortestDistance(int fd, Graph graph) {
    int d=INT_MAX;
    for(int i=0;i<graph.vertexNum();i++){
        for(int j=0;j<i;j++){
            if (graph.at(i,j) > 0 && graph.at(i,j) < d){
                d= graph.at(i,j);
            }
        }
    }
    // send the fd the result
    Util::outputHandler("shortestDistance: " + to_string(d) + "\n",fd);
    return make_pair(fd,graph);

}

pair<int,Graph> GraphAlgo::averageDistance(int fd, Graph graph) {
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
    // send the fd the result
    Util::outputHandler("\naverageDistance: " + to_string(totalDistance/count) +"\n" ,fd);
    return make_pair(fd,graph);
}

void Util::outputHandler(string message, int fd) {
    size_t sendBytes = send(fd, message.c_str(), message.size(), 0);
    if (sendBytes < 0) {
        perror("send");
        exit(1);
    }
}