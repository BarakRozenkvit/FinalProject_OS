//
// Created by Barak Rozenkvit on 04/12/2024.
//
#ifndef FINALPROJECT_OS_HANDLECLIENT_HPP
#define FINALPROJECT_OS_HANDLECLIENT_HPP
#include "MSTAlgo.hpp"
#include <iostream>

using namespace std

namespace ClientHandler{

    void handle(Graph& graph, string command){
        string ans;
        if (command == "Newgraph\n") {
            ans += createNewGraph(graph);
        } else if (command == "MST\n") {
            ans = MSTAlgo::FactoryAlgo::applyAlgo(graph, algo);
        } else if (command == "Newedge\n") {
            int u, v;
            cin >> u >> v;
            graph.addEdge(u,v,w)
            fflush(stdout);
        } else if (command == "Removeedge\n") {
            int u, v;
            cin >> u >> v;
            graph.removeEdge(u, v)
            ans += "Edge removed between " + to_string(u) + " and " + to_string(v) + "\n";
            fflush(stdout);
        } else {
            cout << "Unknown command: " << command << endl;
            fflush(stdout);
        }
        return ans;
    }
}


#endif //FINALPROJECT_OS_HANDLECLIENT_HPP
