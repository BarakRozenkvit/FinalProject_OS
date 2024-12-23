#include "ClientHandler.hpp"

// handlers vector that holds pair of theard id and argument pointer
std::vector<std::pair<pthread_t, void*>> handlers;
// cond for theard is finished
pthread_cond_t condHandler = PTHREAD_COND_INITIALIZER;
// mutex for threads list
pthread_mutex_t mutexHandler = PTHREAD_MUTEX_INITIALIZER;

pthread_t _monitor;

bool _isRunning = true;

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
            // graph->newGraph(size);
            Graph::users_graphs[fd].newGraph(size);
            
        } catch (const std::invalid_argument& e) {
            ClientHandler::outputHandler("Number not provided!", fd);
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

        try {
        //  if (!graph->vertexNum()) {
            if (!Graph::users_graphs[fd].vertexNum()) {
                throw invalid_argument("Cant Calculate on Empty Graph!\n");
            }

            if(!Graph::users_graphs[fd].isConnected()){
                throw invalid_argument("Graph is not connected - cannot find MST\n");
            }


            if (process == "P") {
                // Pipeline implementation
                FactoryPipeline::get(algo)->addTask(fd, Graph::users_graphs[fd]);
            //  FactoryPipeline::get(algo)->addTask(fd, graph->getGraph());
            } else {  // LF - Leader-Follower
                LeaderFollower* lf = LeaderFollowerFactory::get(algo);
                lf->addTask(fd, Graph::users_graphs[fd]);
             // lf->addTask(fd, graph->getGraph());
            }
            sleep(1);

            ClientHandler::outputHandler("Results:\n", fd);
            ClientHandler::inputHandler("Press Any Key to Continue...\n", fd);
            return false;

        } catch (const std::invalid_argument& e) {
            ClientHandler::outputHandler(e.what(), fd);
            sleep(1);
        }

        return false;

        // get edge paramters and add to graph
    } else if (cmd == "ne" || cmd == "Newedge") {
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
            Graph::users_graphs[fd].addEdge(v, u, w);
          //graph->addEdge(v, u, w);
        } catch (const invalid_argument& e) {
            ClientHandler::outputHandler(e.what(), fd);
            sleep(1);
        }

        return false;

        // get edge parameters and remove from graph
    } else if (cmd == "re" || cmd == "Removeedge") {
        string input = ClientHandler::inputHandler("Remove Edge:\n[format: v u]\nYour Input: ", fd);
        stringstream inputStream(input);
        string vstr, ustr;
        inputStream >> vstr;
        inputStream >> ustr;
        try {
            int v = stoi(vstr);
            int u = stoi(ustr);
            Graph::users_graphs[fd].removeEdge(v, u);
         // graph->removeEdge(v, u);
        } catch (const invalid_argument& e) {
            ClientHandler::outputHandler(e.what(), fd);
            sleep(1);
        }
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
        Graph::users_graphs.erase(fd);
        // close(fd);
        return "Exit";
    }
    if (receiveBytes < 0) {
        Graph::users_graphs.erase(fd);
        perror("recv");
        return "Exit";
    }

    buffer[receiveBytes - 1] = '\0';
    string input(buffer);
    if (input == "Exit") {
        Graph::users_graphs.erase(fd);
        // close(fd);
    }

    return input;
}

void* ClientHandler::handleClient(int fd) {
    cout << "Starting Client " <<fd<< endl;
    // while handle graph is true, keep on
    bool exit = false;
    while (_isRunning && !exit){
        exit = ClientHandler::handleGraph(fd);
    }
    // if finished handle graph cond for exiting function
    cout << "Finished Client " <<fd<< endl;
    pthread_cond_signal(&condHandler);
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
    cout << "Started monitor "<< endl;
    while (_isRunning) {
        // wait for thread is finished
        pthread_cond_wait(&condHandler, &mutexHandler);
        if(!_isRunning){
            cout << "In monitor: Finished monitor "<< endl;
            return nullptr;
        }
        for (auto it = handlers.begin(); it != handlers.end();) {
            proactorArgsClient* data = static_cast<proactorArgsClient*>((*it).second);
            // get args of thread, if finished stop it and erase from list
            if (data->pause) {
                cout << "in monitor: Stopping clint " <<data->sockfd<< endl;
                stopProactorClient((*it).first, data);
                handlers.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void ClientHandler::startMonitorHandlers() {
    // start thread for monitor handlers and add to handlers
    int ret = pthread_create(&_monitor, nullptr, ClientHandler::monitorHandlers, nullptr);
    if (ret != 0) {
        perror("pthread_create");
    }
}

void ClientHandler::killHandlers() {
    _isRunning = false;
    pthread_mutex_lock(&mutexHandler);
    for (auto id : handlers) {
        // for every thread in list stop the handlers
        proactorArgsClient* data = static_cast<proactorArgsClient*>(id.second);
        cout << "in Kill Stopping clint " <<data->sockfd<< endl;
        stopProactorClient(id.first, data);
    }
    pthread_mutex_unlock(&mutexHandler);
    pthread_cond_signal(&condHandler);
    cout << "in Kill: Stopping monitor "<< endl;
    pthread_join(_monitor, nullptr);
}
