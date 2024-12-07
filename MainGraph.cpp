#include "MainGraph.hpp"

MainGraph* MainGraph::instance = nullptr;
pthread_mutex_t MainGraph::mutex =PTHREAD_MUTEX_INITIALIZER;

MainGraph::MainGraph() {};
MainGraph::~MainGraph(){};

MainGraph* MainGraph::getInstance(){
    pthread_mutex_lock(&MainGraph::mutex);
    if (MainGraph::instance == nullptr){
        MainGraph::instance = new MainGraph();
    }
    pthread_mutex_unlock(&MainGraph::mutex);
    return MainGraph::instance;
}
void MainGraph::destroyInstance(){
    pthread_mutex_lock(&MainGraph::mutex);
    delete MainGraph::instance;
    MainGraph::instance = nullptr;
    pthread_mutex_unlock(&MainGraph::mutex);
}

void MainGraph::lockInstance() {
    pthread_mutex_lock(&MainGraph::mutex);
}

void MainGraph::unlockInstance() {
    pthread_mutex_unlock(&MainGraph::mutex);
}