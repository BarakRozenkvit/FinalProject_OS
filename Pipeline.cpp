#include "Pipeline.hpp"

std::vector<std::pair<pthread_t, void*>> workers;
pthread_cond_t condWorker = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexWorker = PTHREAD_MUTEX_INITIALIZER;

Pipeline::Pipeline(pair<int,Graph> (*mstAlgo) (int,Graph)){    
    cout << "Starting new Pipeline" << endl;
    _stage = new ActiveObject(mstAlgo,
                new ActiveObject(GraphAlgo::getTotalWeight,
                new ActiveObject(DistanceAlgo::FloyedWarshall,
                new ActiveObject(GraphAlgo::averageDistance,
                new ActiveObject(GraphAlgo::longestDistance,
                new ActiveObject(GraphAlgo::shortestDistance,
                nullptr))))));
    execute();
}

Pipeline::~Pipeline(){
    cout << "Destorying Pipeline" << endl;
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
        pair<pthread_t, void*> id = startProactorPipeline(current ,Pipeline::runStage);
        pthread_mutex_lock(&mutexWorker);
        workers.push_back(id);
        pthread_mutex_unlock(&mutexWorker);        
        current = current->getNext();
    }
};

void Pipeline::addTask(int fd, Graph graph){
    cout << "Adding new graph to pipeline, request from: " << fd <<endl;
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
        pthread_kill(id.first, 0);
        proactorArgsPipeline* data = static_cast<proactorArgsPipeline*>(id.second);
        free(data->stage);
        free(data);
    }
    pthread_mutex_unlock(&mutexWorker);
    Singletone<PipelinePrim>::destroyInstance();
    Singletone<PipelineKruskal>::destroyInstance();
}