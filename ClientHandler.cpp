#include "ClientHandler.hpp"

// handlers vector that holds pair of theard id and argument pointer
std::vector<std::pair<pthread_t, void*>> handlers;
// cond for theard is finished
pthread_cond_t condHandler = PTHREAD_COND_INITIALIZER;
// mutex for threads list
pthread_mutex_t mutexHandler = PTHREAD_MUTEX_INITIALIZER;

pthread_t _monitor;

bool _isRunning = true;

pthread_mutex_t isRunningMutex = PTHREAD_MUTEX_INITIALIZER;

bool ClientHandler::handleGraph(int fd) {
    ClientHandler::outputHandler("Requesting Permission to Graph...\n", fd);

    string cmd = ClientHandler::inputHandler(
        "\nWrite a Command:"
        "\n- Newgraph [to create new graph]"
        "\n- MST (calculate mst to graph)"
        "\n- Newedge (add edge)"
        "\n- Removeedge (remove edge)"
        "\n- Exit"
        "\nYour Input: ",
        fd);

    // get number and create new graph
    if (cmd == "ng" || cmd == "Newgraph") {
        string number = ClientHandler::inputHandler("Choose graph size\nYour Input: ", fd);
        try {
            int size = stoi(number);
            if (size < 0) {
                throw invalid_argument("Negative number not allowed!\n");  // Throw correct exception
            }
            pthread_mutex_lock(&Graph::graph_mutex);
            Graph::users_graphs[fd].newGraph(size);
            pthread_mutex_unlock(&Graph::graph_mutex);

        } catch (const std::invalid_argument& e) {
            ClientHandler::outputHandler(e.what(), fd);  // Output the correct exception message
            sleep(1);
        } catch (const std::out_of_range& e) {
            ClientHandler::outputHandler("Number must be lower than int!", fd);  // Handle out of range exception
            sleep(1);
        }
        return false;
    }

    // get algo type and show results
    else if (cmd == "MST") {
        string process = ClientHandler::inputHandler(
            "Input wanted work process:\nLF or P?\nYour Input: ", fd);

        if (process != "LF" && process != "P") {
            ClientHandler::outputHandler("Invalid work process! Use LF or P\n", fd);
            sleep(1);

            return false;
        }

        string algo = ClientHandler::inputHandler(
            "Choose MST algorithm\n- Prim\n- Kruskal\nYour Input: ", fd);

        pthread_mutex_lock(&Graph::graph_mutex);
        ClientHandler::outputHandler("Results:\n", fd);
        try {
            //  if (!graph->vertexNum()) {
            if (!Graph::users_graphs[fd].vertexNum()) {
                throw invalid_argument("Cant Calculate on Empty Graph!\n");
            }

            if (!Graph::users_graphs[fd].isConnected()) {
                throw invalid_argument("Graph is not connected - cannot find MST\n");
            }

            if (process == "P") {
                // Pipeline implementation
                FactoryPipeline::get(algo)->addTask(fd, Graph::users_graphs[fd]);
            } else {  // LF - Leader-Follower
                LeaderFollower* lf = LeaderFollowerFactory::get(algo);
                lf->addTask(fd, Graph::users_graphs[fd]);
            }
            sleep(1);

            // ClientHandler::inputHandler("Press Any Key to Continue...\n", fd);
            pthread_mutex_unlock(&Graph::graph_mutex);
            return false;

        } catch (const std::invalid_argument& e) {
            ClientHandler::outputHandler(e.what(), fd);
            sleep(1);
        }
        pthread_mutex_unlock(&Graph::graph_mutex);

        return false;

        // get edge paramters and add to graph
    } else if (cmd == "ne" || cmd == "Newedge") {
        string input = ClientHandler::inputHandler("Add Edge:\n[format: v u w]\nYour Input: ", fd);
        stringstream inputStream(input);
        string vstr, ustr, wstr;
        inputStream >> vstr;
        inputStream >> ustr;
        inputStream >> wstr;
        pthread_mutex_lock(&Graph::graph_mutex);
        try {
            int v = stoi(vstr);
            int u = stoi(ustr);
            int w = stoi(wstr);
            Graph::users_graphs[fd].addEdge(v, u, w);
            // graph->addEdge(v, u, w);
        } catch (const invalid_argument& e) {
            ClientHandler::outputHandler(e.what(), fd);
            sleep(1);
        }

        pthread_mutex_unlock(&Graph::graph_mutex);
        return false;

        // get edge parameters and remove from graph
    } else if (cmd == "re" || cmd == "Removeedge") {
        string input = ClientHandler::inputHandler("Remove Edge:\n[format: v u]\nYour Input: ", fd);
        stringstream inputStream(input);
        string vstr, ustr;
        inputStream >> vstr;
        inputStream >> ustr;
        pthread_mutex_lock(&Graph::graph_mutex);
        try {
            int v = stoi(vstr);
            int u = stoi(ustr);
            Graph::users_graphs[fd].removeEdge(v, u);
            // graph->removeEdge(v, u);
        } catch (const invalid_argument& e) {
            ClientHandler::outputHandler(e.what(), fd);
            sleep(1);
        }
        pthread_mutex_unlock(&Graph::graph_mutex);
        return false;

        // if exit set flag
    } else if (cmd == "Exit") {
        return true;

        // if unknown command
    } else {
        ClientHandler::outputHandler("Unknown command!", fd);
        sleep(1);
        return false;
    }
}

void* ClientHandler::handleConnection(int fd) {
    struct sockaddr_in client_address;
    socklen_t clientAddressLen = sizeof(client_address);
    memset(&client_address, 0, sizeof(client_address));
    clientAddressLen = sizeof(client_address);
    // accept new client
    int client_fd = accept(fd, (struct sockaddr*)&client_address, &clientAddressLen);
    if (client_fd == -1) {
        perror("accept");
        return new int(-1);
    }
    // create a thread client and add to handlers
    pair<pthread_t, void*> id = startProactorClient(client_fd, ClientHandler::handleClient);
    pthread_mutex_lock(&mutexHandler);
    handlers.push_back(id);
    pthread_mutex_unlock(&mutexHandler);
    return new int(client_fd);
}

string ClientHandler::inputHandler(string message, int fd) {
    // send messege to fd
    size_t sendBytes = send(fd, message.c_str(), message.size(), 0);
    if (sendBytes < 0) {
        perror("send");
        exit(1);
    }

    // receive from fd
    char buffer[256] = {'\0'};
    size_t receiveBytes = recv(fd, buffer, sizeof(buffer), 0);
    if (receiveBytes == 0) {
        pthread_mutex_lock(&Graph::graph_mutex);
        Graph::users_graphs.erase(fd);
        pthread_mutex_unlock(&Graph::graph_mutex);
        return "Exit";
    }
    if (receiveBytes < 0) {
        pthread_mutex_lock(&Graph::graph_mutex);
        Graph::users_graphs.erase(fd);
        pthread_mutex_unlock(&Graph::graph_mutex);
        perror("recv");
        return "Exit";
    }

    buffer[receiveBytes - 1] = '\0';
    string input(buffer);
    if (input == "Exit") {
        pthread_mutex_lock(&Graph::graph_mutex);
        Graph::users_graphs.erase(fd);
        pthread_mutex_unlock(&Graph::graph_mutex);
    }

    return input;
}

void* ClientHandler::handleClient(int fd) {
    bool exit = false;

    // Lock the mutex before reading _isRunning
    pthread_mutex_lock(&isRunningMutex);
    while (_isRunning && !exit) {
        pthread_mutex_unlock(&isRunningMutex);  // Unlock before calling handleGraph
        exit = ClientHandler::handleGraph(fd);

        // Lock the mutex again before checking _isRunning
        pthread_mutex_lock(&isRunningMutex);
    }
    pthread_mutex_unlock(&isRunningMutex);  // Unlock after exiting the loop

    if (exit && _isRunning) {
        pthread_mutex_lock(&mutexHandler);
        pthread_cond_signal(&condHandler);
        pthread_mutex_unlock(&mutexHandler);
    }
    return nullptr;
}

void ClientHandler::outputHandler(string message, int fd) {
    // send messege to fd
    size_t sendBytes = send(fd, message.c_str(), message.size(), 0);
    if (sendBytes < 0) {
        perror("send");
        exit(1);
    }
}

void* ClientHandler::monitorHandlers(void*) {
    pthread_mutex_lock(&isRunningMutex);
    bool run = _isRunning;
    pthread_mutex_unlock(&isRunningMutex);

    while (run) {
        pthread_mutex_lock(&mutexHandler);
        pthread_cond_wait(&condHandler, &mutexHandler);
        for (auto it = handlers.begin(); it != handlers.end();) {
            proactorArgsClient* data = static_cast<proactorArgsClient*>((*it).second);

            pthread_mutex_lock(&pauseMutex);  // Lock to safely access `pause`
            bool isPaused = data->pause;
            pthread_mutex_unlock(&pauseMutex);  // Unlock after accessing `pause`

            if (isPaused) {
                stopProactorClient((*it).first, data);
                handlers.erase(it);
            } else {
                ++it;
            }
        }
        pthread_mutex_unlock(&mutexHandler);
        pthread_mutex_lock(&isRunningMutex);
        run = _isRunning;
        pthread_mutex_unlock(&isRunningMutex);
    }
    return nullptr;
}

void ClientHandler::startMonitorHandlers() {
    // start thread for monitor handlers and add to handlers
    int ret = pthread_create(&_monitor, nullptr, ClientHandler::monitorHandlers, nullptr);
    if (ret != 0) {
        perror("pthread_create");
    }
}

void ClientHandler::killHandlers() {
    pthread_mutex_lock(&isRunningMutex);
    _isRunning = false;  // Safely set _isRunning to false
    pthread_mutex_unlock(&isRunningMutex);

    pthread_mutex_lock(&mutexHandler);
    for (auto id : handlers) {
        proactorArgsClient* data = static_cast<proactorArgsClient*>(id.second);
        stopProactorClient(id.first, data);
    }
    pthread_cond_signal(&condHandler);  // Signal only once
    pthread_mutex_unlock(&mutexHandler);
}
