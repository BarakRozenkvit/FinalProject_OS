//
// Created by Barak Rozenkvit on 04/12/2024.
//
//
#include "HandleClient.hpp"

ClientHandler::
static string Kosaraju::handle_client_command(vector<list<int>>& adj, string command) {
    string ans;
    if (command == "Newgraph\n") {
        ans += "Creating new graph\n";
        ans += createNewGraph(adj);
    } else if (command == "Kosaraju\n") {
        int n = adj.size() - 1;
        ans = Kosaraju_list(n, adj);
    } else if (command == "Newedge\n") {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        ans += "New edge added between " + to_string(u) + " and " + to_string(v) + "\n";
        fflush(stdout);
    } else if (command == "Removeedge\n") {
        int u, v;
        cin >> u >> v;
        auto it = find(adj[u].begin(), adj[u].end(), v);
        if (it != adj[u].end()) {
            adj[u].erase(it);
            ans += "Edge removed between " + to_string(u) + " and " + to_string(v) + "\n";
            fflush(stdout);
        }
    } else {
        cout << "Unknown command: " << command << endl;
        fflush(stdout);
    }
    return ans;
}
