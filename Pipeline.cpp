#include "Pipeline.hpp"

Pipeline::Pipeline(Graph (*mstAlgo) (Graph*)){
    _stage = new ActiveObject(mstAlgo,
                new ActiveObject(GraphAlgo::getTotalWeight,
                new ActiveObject(DistanceAlgo::FloyedWarshall,
                new ActiveObject(GraphAlgo::averageDistance,
                new ActiveObject(GraphAlgo::longestDistance,
                new ActiveObject(GraphAlgo::shortestDistance,
                nullptr))))));
    execute();
}

void Pipeline::execute(){
    pthread_t thread;
    ActiveObject* _current = _stage;
    while(_current!=nullptr){
        int ret = pthread_create(&thread, nullptr, Pipeline::runStage, static_cast<void*>(_current));
        if (ret != 0) {
            perror("pthread_create");
        }
        _current = _current->getNext();
    }
};

void Pipeline::addTask(Graph* graph){
    cout << "Adding new graph to pipeline, Address: " << graph <<endl;
    _stage->pushTask(graph);
}

void* Pipeline::runStage(void* stage){
    ActiveObject* thisStage = static_cast<ActiveObject*>(stage);
    thisStage->run();
}