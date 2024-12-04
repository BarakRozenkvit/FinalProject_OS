//
// Created by Barak Rozenkvit on 04/12/2024.
//

#ifndef FINALPROJECT_OS_HANDLECLIENT_HPP
#define FINALPROJECT_OS_HANDLECLIENT_HPP
#include "MSTAlgo.hpp"

namespace ClientHandler{

    void handle(Graph& graph, string command){
        string ans;
        if (command == "Newgraph\n") {
            ans += createNewGraph(graph);
        } else if (command == "MST\n") {
            ans = MSTAlgo::FactoryAlgo::applyAlgo(n, graph);
        } else if (command == "Newedge\n") {
            int u, v;
            cin >> u >> v;
            graph.addEdge(u,v,w)
            ans += "New edge added between " + to_string(u) + " and " + to_string(v) + "\n";
            fflush(stdout);
        } else if (command == "Removeedge\n") {
            int u, v;
            cin >> u >> v;
            auto it = find(graph[u].begin(), graph[u].end(), v);
            if (it != graph[u].end()) {
                graph[u].erase(it);
                ans += "Edge removed between " + to_string(u) + " and " + to_string(v) + "\n";
                fflush(stdout);
            }
        } else {
            cout << "Unknown command: " << command << endl;
            fflush(stdout);
        }
        return ans;
    }




}


#endif //FINALPROJECT_OS_HANDLECLIENT_HPP
