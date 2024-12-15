#include "ClientHandler.hpp"

// handlers vector that holds pair of theard id and argument pointer
std::vector<std::pair<pthread_t, void*>> handlers;
// cond for theard is finished
pthread_cond_t condHandler = PTHREAD_COND_INITIALIZER;
// mutex for threads list
pthread_mutex_t mutexHandler = PTHREAD_MUTEX_INITIALIZER;

string CLEAR_TERMINAL = "\033[2J\033[H";

int ClientHandler::handleGraph(int fd) {
    ClientHandler::outputHandler("Requesting Permission to Graph...\n", fd);
    // get instance of main graph
    Graph* graph = MainGraph::getInstance();
    // lock the main graph
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
    
    // get number and create new graph
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
    
    // get algo type and show results
    else if (cmd == "MST") {        
        string algo = ClientHandler::inputHandler("Choose MST algorithm\n- Prim\n- Kruskal\nYour Input: ", fd);
        try{
            if (!graph->vertexNum()){
                throw invalid_argument("Cant Calculate on Empty Graph!\n");
            }

            ClientHandler::outputHandler("Results:\n", fd); 
            // call factory pipeline, give fd of client and deep copy of graph
            FactoryPipeline::get(algo)->addTask(fd, graph->getGraph());
            // unlock the graph
            MainGraph::unlockInstance();
            sleep(2);
            ClientHandler::inputHandler("Press Any Key to Continue...\n", fd);
            return !exit;
        }
        catch (const std::invalid_argument& e) {
            ClientHandler::outputHandler(e.what(), fd);
            sleep(1);
        }

    // get edge paramters and add to graph
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

    // get edge parameters and remove from graph
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
    
    // if exit set flag
    } else if (cmd == "Exit") {
        exit = 1;
    
    // if unknown command
    } else {
        ClientHandler::outputHandler("Unknown command!", fd);
        sleep(1);
    }
    // unlock main graph
    MainGraph::unlockInstance();
    return !exit;
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
    // while handle graph is true, keep on
    while (ClientHandler::handleGraph(fd)) {
    }
    // if finished handle graph cond for exiting function
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

void* ClientHandler::monitorHandlers(int) {
    while (1) {
        // wait for thread is finished
        pthread_cond_wait(&condHandler, &mutexHandler);
        for (auto it = handlers.begin(); it != handlers.end();) {
            proactorArgsClient* data = static_cast<proactorArgsClient*>((*it).second);
            // get args of thread, if finished stop it and erase from list
            if (data->pause) {
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
    pair<pthread_t, void*> id = startProactorClient(0, ClientHandler::monitorHandlers);
    pthread_mutex_lock(&mutexHandler);
    handlers.push_back(id);
    pthread_mutex_unlock(&mutexHandler);
}

void ClientHandler::killHandlers() {
    pthread_mutex_lock(&mutexHandler);
    for (auto id : handlers) {
        // for every thread in list stop the handlers
        pthread_kill(id.first, 0);
        proactorArgsClient* data = static_cast<proactorArgsClient*>(id.second);
        close(data->sockfd);
        free(data);
    }
    pthread_mutex_unlock(&mutexHandler);
}
