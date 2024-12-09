#include "ClientHandler.hpp"


int ClientHandler::handleGraph(int fd) {
    ClientHandler::outputHandler("\033[2J\033[H",fd);
    ClientHandler::outputHandler("Requesting Permission to Graph...\n",fd);
    MainGraph* graph = MainGraph::getInstance();
    MainGraph::lockInstance();
    ClientHandler::outputHandler("Permission Granted!\n",fd);
    string cmd = ClientHandler::inputHandler("Write a Command:"
                                             "\n- Newgraph [to create new graph]"
                                             "\n- MST (calculate mst to graph)"
                                             "\n- "
                                             "Newedge (add edge)"
                                             "\n- Removeedge (remove edge)"
                                             "\nYour Input: ",fd);
    int exit = 0;
    if (cmd == "Newgraph") {
        ClientHandler::outputHandler("\033[2J\033[H",fd);
        string number = ClientHandler::inputHandler("Choose graph size\nYour Input: ", fd);
        try {
            int size = stoi(number);
            graph->newGraph(size);
        }
        catch (const std::invalid_argument& e) {
            ClientHandler::outputHandler("\033[2J\033[H",fd);
            ClientHandler::outputHandler("Number not provided!",fd);
            sleep(1);
        }
    }
    else if (cmd == "MST") {
        ClientHandler::outputHandler("\033[2J\033[H",fd);
        string algo = ClientHandler::inputHandler("Choose MST algorithm\n- Prim\n- Kruskal\nYour Input: ", fd);
        try {
            MSTAlgo::FactoryAlgo::applyAlgo(algo);
        }
        catch (const invalid_argument& e){
            ClientHandler::outputHandler("\033[2J\033[H",fd);
            ClientHandler::outputHandler(e.what(),fd);
            sleep(1);
        }
    }
    else if (cmd == "Newedge") {
        ClientHandler::outputHandler("\033[2J\033[H",fd);
        ClientHandler::outputHandler("Add Edge:\n[format: v u w]\nYour Input:",fd);
        string input = ClientHandler::inputHandler("Add Edge:\n[format: v u w]\nYour Input: ",fd);
        stringstream inputStream(input);
        string vstr,ustr,wstr;
        inputStream >> vstr;
        inputStream >> ustr;
        inputStream >> wstr;
        try{
            int v = stoi(vstr);
            int u = stoi(ustr);
            int w = stoi(wstr);
            graph->addEdge(v,u,w);
        }
        catch (const invalid_argument& e) {
            ClientHandler::outputHandler("\033[2J\033[H",fd);
            ClientHandler::outputHandler(e.what(),fd);
            sleep(1);
        }
    }
    else if (cmd == "Removeedge") {
        ClientHandler::outputHandler("\033[2J\033[H",fd);
        string input = ClientHandler::inputHandler("Remove Edge:\n[format: v u]\nYour Input: ",fd);
        stringstream inputStream(input);
        string vstr,ustr;
        inputStream >> vstr;
        inputStream >> ustr;
        try{
            int v = stoi(vstr);
            int u = stoi(ustr);
            graph->removeEdge(v,u);
        }
        catch (const invalid_argument& e) {
            ClientHandler::outputHandler("\033[2J\033[H",fd);
            ClientHandler::outputHandler(e.what(),fd);
            sleep(1);
        }
    }
    else if (cmd == "Exit") {
        exit = 1;
    }
    else {
        ClientHandler::outputHandler("Unknown command!",fd);
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
    pair<pthread_t,void*> id = startProactor(client_fd, ClientHandler::handleClient);
    handlers.push_back(id);
    cout << "Create new Thread for Fd: " + to_string(client_fd)  << endl;
    return new int(client_fd);
}

string ClientHandler::inputHandler(string message,int fd) {

    size_t sendBytes = send(fd,message.c_str(), message.size(),0);
    if (sendBytes < 0){
        perror("send");
        exit(1);
    }

    char buffer[256] = {'\0'};
    size_t receiveBytes = recv(fd,buffer,sizeof (buffer),0);
    if(receiveBytes == 0){
        cout << "Exit Theard: Fd " << fd << endl;
        exit(1);
    }
    if (receiveBytes < 0 ){
        return "Exit";
    }

    buffer[receiveBytes - 1] = '\0';
    string input(buffer);

    return input;
}

void* ClientHandler::handleClient(int fd) {
    while (ClientHandler::handleGraph(fd)) {}
    return nullptr;
}

void ClientHandler::outputHandler(string message,int fd) {
    size_t sendBytes = send(fd,message.c_str(), message.size(),0);
    if (sendBytes < 0){
        perror("send");
        exit(1);
    }
}