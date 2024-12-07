//
// Created by Barak Rozenkvit on 04/12/2024.
//
//
#include "HandleClient.hpp"

void ClientHandler::handle(MainGraph* graph, std::string command) {
    MainGraph::lockInstance();
    string ans;
    if (command == "Newgraph\n") {
        graph->newGraph();
    } else if (command == "MST\n") {
        ans = MSTAlgo::FactoryAlgo::applyAlgo(graph, algo);
    } else if (command == "Newedge\n") {
        int u, v;
        cin >> u >> v;
        graph->addEdge(u,v,w)
        fflush(stdout);
    } else if (command == "Removeedge\n") {
        int u, v;
        cin >> u >> v;
        graph->removeEdge(u, v)
        ans += "Edge removed between " + to_string(u) + " and " + to_string(v) + "\n";
        fflush(stdout);
    } else {
        cout << "Unknown command: " << command << endl;
        fflush(stdout);
    }
    MainGraph::unlockInstance();
    return ans;

}
