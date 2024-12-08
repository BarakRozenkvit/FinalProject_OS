//
// Created by Barak Rozenkvit on 04/12/2024.
//
//
#include "ClientHandler.hpp"
#include "Proactor.hpp"

void ClientHandler::handleGraph(MainGraph* graph, string cmd) {
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

string ClientHandler::inputHandler(string message,int fd) {

    char buf[256];
    int nbytes = recv(fd, buf, sizeof buf,0);

    if (nbytes < 0) {
        throw runtime_error("recv");
    }
    else if (nbytes == 0) {
        cout << "Exit Theard: Fd " << fd << endl;
        exit(1);
    }
    else {
        // if has data send it to client handler, dup std in and out to client
        // so every message from him goes to function and from function to him
        buf[nbytes] = '\0';
        string command(buf); // Convert the C-string to a C++ string for easier handling
        int res = dup2(fd, STDIN_FILENO);
        if (res == -1) {
            perror("dup2");
            close(fd);
        }
    }
}

void* ClientHandler::handleClient(MainGraph *graph, int fd) {
    while (1) {
        string input = ClientHandler::inputHandler("Choose",fd);
        string output = ClientHandler::handleGraph(graph, input);  // Process the command and generate a response
        ClientHandler::outputHandler(output);
    }
    return nullptr;
}