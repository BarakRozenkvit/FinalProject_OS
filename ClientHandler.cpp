//
// Created by Barak Rozenkvit on 04/12/2024.
//
//
#include "ClientHandler.hpp"
#include "Proactor.hpp"

void* ClientHandler::handleClient(MainGraph* graph, int fd) {
    MainGraph::lockInstance();
    string cmd = ClientHandler::inputHandler("choose:",fd);
    if (cmd == "Newgraph\n") {
        graph->newGraph(2);
    } else if (cmd == "MST\n") {
        cmd = MSTAlgo::FactoryAlgo::applyAlgo(graph, cmd);
    } else if (cmd == "Newedge\n") {
        int u, v;
        cin >> u >> v;
        graph->addEdge(u,v,w)
        fflush(stdout);
    } else if (cmd == "Removeedge\n") {
        int u, v;
        cin >> u >> v;
        graph->removeEdge(u, v)
        cmd += "Edge removed between " + to_string(u) + " and " + to_string(v) + "\n";
        fflush(stdout);
    } else {
        cout << "Unknown command: " << cmd << endl;
        fflush(stdout);
    }
    MainGraph::unlockInstance();
    return cmd;

}

void* ClientHandler::handleConnection(MainGraph *graph, int fd) {

    struct sockaddr_in client_address;
    socklen_t clientAddressLen = sizeof(client_address);
    memset(&client_address, 0, sizeof(client_address));
    clientAddressLen = sizeof(client_address);
    int client_fd = accept(fd, (struct sockaddr*)&client_address, &clientAddressLen);
    if (client_fd == -1) {
        perror("accept");
        return new int(-1);
    }
    cout << "Client connected, Fd: " << client_fd << endl;
    pthread_t id = startProactor(graph ,client_fd, ClientHandler::handleClient);
    handlers.push_back(id);
    cout << "Create new Thread for Fd: " << client_fd << endl;
    return new int(client_fd);

}

string ClientHandler::inputHandler(string message,int fd) {}
