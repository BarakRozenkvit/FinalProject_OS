#include "Pipeline.hpp"


Pipeline::Pipeline(pair<int,Graph> (*mstAlgo) (int,Graph)){   
    _isRunning = true;
    pthread_mutex_init(&mutexWorker, nullptr);
    pthread_cond_init(&condWorker, nullptr);
    std::vector<pthread_t> workers;
    // create the pipeline list of workers 
    _stage = new ActiveObject(mstAlgo,
                new ActiveObject(GraphAlgo::getTotalWeight,
                new ActiveObject(DistanceAlgo::FloydWarshall,
                new ActiveObject(GraphAlgo::averageDistance,
                new ActiveObject(GraphAlgo::longestDistance,
                new ActiveObject(GraphAlgo::shortestDistance,
                nullptr))))));
    // create threads
    execute();
}

Pipeline::~Pipeline(){
    // destroy all allocated active objects
    ActiveObject* current = _stage;
    stop();
    while(current!=nullptr){
        ActiveObject* temp = current;
        current = current->getNext();
        delete temp; 
    }    
}


void Pipeline::execute(){
    pthread_t thread;
    ActiveObject* current = _stage;
    while(current!=nullptr){
        // create thread and add to list
        pthread_t thread;
        int ret = pthread_create(&thread, nullptr, Pipeline::runStage, current);
        if (ret != 0) {
            perror("pthread_create");
        }
        pthread_mutex_lock(&mutexWorker);
        workers.push_back(thread);
        pthread_mutex_unlock(&mutexWorker);        
        current = current->getNext();
    }
};

void Pipeline::addTask(int fd, Graph graph){
    _stage->pushTask(make_pair(fd, graph));
}

void* Pipeline::runStage(void* stage){
    ActiveObject* thisStage = static_cast<ActiveObject*>(stage);
    thisStage->run();
    return nullptr;
}


void Pipeline::stop(){
    _isRunning = false;
    pthread_mutex_lock(&mutexWorker);
    for (pthread_t thread : workers) {  // Wait for all threads to finish
        pthread_join(thread, nullptr);
    }
    workers.clear();
    pthread_mutex_unlock(&mutexWorker);
}

void Pipeline::destroyAll() {
    // pthread_mutex_lock(&mutexWorker);
    // pthread_mutex_unlock(&mutexWorker);
    // destroy pipelines
    Singletone<PipelinePrim>::destroyInstance();
    Singletone<PipelineKruskal>::destroyInstance();
}