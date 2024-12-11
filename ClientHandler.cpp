#include "ClientHandler.hpp"

std::vector<std::pair<pthread_t, void*>> handlers;  // Definition
pthread_cond_t condHandler = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexHandler = PTHREAD_MUTEX_INITIALIZER;

string CLEAR_TERMINAL = "\033[2J\033[H";

int ClientHandler::handleGraph(int fd) {
    ClientHandler::outputHandler("Requesting Permission to Graph...\n", fd);
    MainGraph* graph = MainGraph::getInstance();
    MainGraph::lockInstance();
    string cmd = ClientHandler::inputHandler(
                                                 "\nWrite a Command:"
                                                 "\n- Newgraph [to create new graph]"
                                                 "\n- MST (calculate mst to graph)"
                                                 "\n- Newedge (add edge)"
                                                 "\n- Removeedge (remove edge)"
                                                 "\n- Exit"
                                                 "\nYour Input: ",
                                             fd);
    int exit = 0;
    if (cmd == "Newgraph") {
        string number = ClientHandler::inputHandler("Choose graph size\nYour Input: ", fd);
        try {
            int size = stoi(number);
            graph->newGraph(size);
        } catch (const std::invalid_argument& e) {
            ClientHandler::outputHandler("Number not provided!", fd);
            sleep(1);
        }
    }
    // else if (cmd == "MST") {
    //     string algo = ClientHandler::inputHandler(CLEAR_TERMINAL + "Choose MST algorithm\n- Prim\n- Kruskal\nYour Input: ", fd);
    //     try {
    //         MSTAlgo::FactoryAlgo::applyAlgo(graph, algo);
    //     }
    //     catch (const invalid_argument& e){
    //         ClientHandler::outputHandler(CLEAR_TERMINAL + e.what(),fd);
    //         sleep(1);
    //     }
    // }
    else if (cmd == "MST") {
        string algo = ClientHandler::inputHandler("Choose MST algorithm\n- Prim\n- Kruskal\nYour Input: ", fd);
        try {
            auto [mst, details] = MSTAlgo::FactoryAlgo::applyAlgoWithDetails(graph, algo);

            // Output the MST details to the client.
            ClientHandler::outputHandler("MST Details:\n" + details, fd);
        } catch (const std::invalid_argument& e) {
            ClientHandler::outputHandler(e.what(), fd);
            sleep(1);
        }
    } else if (cmd == "Newedge") {
        string input = ClientHandler::inputHandler("Add Edge:\n[format: v u w]\nYour Input: ", fd);
        stringstream inputStream(input);
        string vstr, ustr, wstr;
        inputStream >> vstr;
        inputStream >> ustr;
        inputStream >> wstr;
        try {
            int v = stoi(vstr);
            int u = stoi(ustr);
            int w = stoi(wstr);
            graph->addEdge(v, u, w);
        } catch (const invalid_argument& e) {
            ClientHandler::outputHandler(e.what(), fd);
            sleep(1);
        }
    } else if (cmd == "Removeedge") {
        string input = ClientHandler::inputHandler("Remove Edge:\n[format: v u]\nYour Input: ", fd);
        stringstream inputStream(input);
        string vstr, ustr;
        inputStream >> vstr;
        inputStream >> ustr;
        try {
            int v = stoi(vstr);
            int u = stoi(ustr);
            graph->removeEdge(v, u);
        } catch (const invalid_argument& e) {
            ClientHandler::outputHandler(e.what(), fd);
            sleep(1);
        }
    } else if (cmd == "Exit") {
        exit = 1;
    } else {
        ClientHandler::outputHandler("Unknown command!", fd);
        sleep(1);
    }
    MainGraph::unlockInstance();
    return !exit;
}

void* ClientHandler::handleConnection(int fd) {
    struct sockaddr_in client_address;
    socklen_t clientAddressLen = sizeof(client_address);
    memset(&client_address, 0, sizeof(client_address));
    clientAddressLen = sizeof(client_address);
    int client_fd = accept(fd, (struct sockaddr*)&client_address, &clientAddressLen);
    if (client_fd == -1) {
        perror("accept");
        return new int(-1);
    }
    pair<pthread_t, void*> id = startProactor(client_fd, ClientHandler::handleClient);
    pthread_mutex_lock(&mutexHandler);
    handlers.push_back(id);
    pthread_mutex_unlock(&mutexHandler);
    return new int(client_fd);
}

string ClientHandler::inputHandler(string message, int fd) {
    size_t sendBytes = send(fd, message.c_str(), message.size(), 0);
    if (sendBytes < 0) {
        perror("send");
        exit(1);
    }

    char buffer[256] = {'\0'};
    size_t receiveBytes = recv(fd, buffer, sizeof(buffer), 0);
    if (receiveBytes == 0) {
        close(fd);
        return "Exit";
    }
    if (receiveBytes < 0) {
        perror("recv");
        return "Exit";
    }

    buffer[receiveBytes - 1] = '\0';
    string input(buffer);
    if (input == "Exit") {
        close(fd);
    }

    return input;
}

void* ClientHandler::handleClient(int fd) {
    while (ClientHandler::handleGraph(fd)) {
    }
    pthread_cond_signal(&condHandler);
    return nullptr;
}

void ClientHandler::outputHandler(string message, int fd) {
    size_t sendBytes = send(fd, message.c_str(), message.size(), 0);
    if (sendBytes < 0) {
        perror("send");
        exit(1);
    }
}

void* ClientHandler::monitorHandlers(int) {
    while (1) {
        pthread_cond_wait(&condHandler, &mutexHandler);
        for (auto it = handlers.begin(); it != handlers.end();) {
            proactorArgs* data = static_cast<proactorArgs*>((*it).second);
            if (data->pause) {
                stopProactor((*it).first, data);
                handlers.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void ClientHandler::startMonitorHandlers() {
    pair<pthread_t, void*> id = startProactor(0, ClientHandler::monitorHandlers);
    pthread_mutex_lock(&mutexHandler);
    handlers.push_back(id);
    pthread_mutex_unlock(&mutexHandler);
}

void ClientHandler::killHandlers(int signal) {
    if (signal == SIGINT || signal == SIGKILL) {
        std::cout << "shutting down gracefully..." << std::endl;
        pthread_mutex_lock(&mutexHandler);
        for (auto id : handlers) {
            pthread_kill(id.first, 0);
            proactorArgs* data = static_cast<proactorArgs*>(id.second);
            close(data->sockfd);
            free(data);
        }
        pthread_mutex_unlock(&mutexHandler);
        exit(0);
    }
}
