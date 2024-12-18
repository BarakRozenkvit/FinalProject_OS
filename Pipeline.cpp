#include "Pipeline.hpp"

std::vector<std::pair<pthread_t, void*>> workers;
pthread_cond_t condWorker = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexWorker = PTHREAD_MUTEX_INITIALIZER;

Pipeline::Pipeline(pair<int,Graph> (*mstAlgo) (int,Graph)){   
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
        pair<pthread_t, void*> id = startProactorPipeline(current ,Pipeline::runStage);
        pthread_mutex_lock(&mutexWorker);
        workers.push_back(id);
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

void Pipeline::killWorkers() {
    pthread_mutex_lock(&mutexWorker);
    for (auto id : workers) {
        // kill all threads and release allocated data
        proactorArgsPipeline* data = static_cast<proactorArgsPipeline*>(id.second);
        stopProactorPipeline(id.first,data);
    }
    pthread_mutex_unlock(&mutexWorker);
    // destroy pipelines
    Singletone<PipelinePrim>::destroyInstance();
    Singletone<PipelineKruskal>::destroyInstance();
}