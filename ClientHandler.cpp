//
// Created by Barak Rozenkvit on 04/12/2024.
//
//
#include "ClientHandler.hpp"


string ClientHandler::handleGraph(int fd) {
    MainGraph* graph = MainGraph::getInstance();
    MainGraph::lockInstance();
    string cmd = ClientHandler::inputHandler("Choose",fd);
    if (cmd == "Newgraph") {
        graph->newGraph(2);
    } else if (cmd == "MST") {
        MSTAlgo::FactoryAlgo::applyAlgo(cmd);
    } else if (cmd == "Newedge") {
        int u, v, w;
        cin >> u >> v >> w;
        graph->addEdge(u,v,w);
        fflush(stdout);
    } else if (cmd == "Removeedge") {
        int u, v;
        cin >> u >> v;
        graph->removeEdge(u, v);
        cmd += "Edge removed between " + to_string(u) + " and " + to_string(v) + "\n";
        fflush(stdout);
    } else {
        cout << "Unknown command: " << cmd << endl;
        fflush(stdout);
    }
    MainGraph::unlockInstance();
    return cmd;

}

void* ClientHandler::handleConnection(int fd) {
    MainGraph::getInstance();
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
    pthread_t id = startProactor(client_fd, ClientHandler::handleClient);
    handlers.push_back(id);
    cout << "Create new Thread for Fd: " << client_fd << endl;
    return new int(client_fd);

}

string ClientHandler::inputHandler(string message,int fd) {

    int in = dup2(fd, STDIN_FILENO);int out = dup2(fd,STDOUT_FILENO);
    if (in == -1 || out == -1) {
        perror("dup2");
        close(fd);
    }

    cout << message << endl;

    string input;
    cin >> input;

    if (input == ""){
        cout << "Exit Theard: Fd " << fd << endl;
        exit(1);
    }

    return input;
}

void* ClientHandler::handleClient(int fd) {
    MainGraph* graph = MainGraph::getInstance();
    while (1) {
        string output = ClientHandler::handleGraph(fd);  // Process the command and generate a response
    }
    return nullptr;
}

void ClientHandler::outputHandler(string message,int fd) {
    int out = dup2(fd,STDOUT_FILENO);
    if (out == -1) {
        perror("dup2");
        close(fd);
    }

    cout << message << endl;
}