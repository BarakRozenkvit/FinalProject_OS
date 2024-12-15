#include <vector>
#include <pthread.h>
#include "ActiveObject.hpp"
#include "Algo.hpp"
#include "Graph.hpp"
#include "Proactor.hpp"

#pragma once

using namespace std;

extern std::vector<std::pair<pthread_t,void*>> workers; // Declaration
extern pthread_cond_t condWorker;
extern pthread_mutex_t mutexWorker;


class Pipeline{

    ActiveObject* _stage;
    
public:
    Pipeline(pair<int,Graph> (*mstAlgo) (int, Graph));
    virtual ~Pipeline();

    void execute();

    void addTask(int fd, Graph graph);

    static void* runStage(void* stage);

    static void killWorkers();
};


class PipelinePrim: public Pipeline{

public:
    PipelinePrim(): Pipeline(MSTAlgo::Prim){};
    ~PipelinePrim() override {};
};


class PipelineKruskal: public Pipeline{

public:
    PipelineKruskal(): Pipeline(MSTAlgo::Kruskal){};
    ~PipelineKruskal() override {};
};


class FactoryPipeline{
public:
    static Pipeline* get(string algo){
        if (algo=="Prim"){
            PipelinePrim* g = Singletone<PipelinePrim>::getInstance();
            return static_cast<Pipeline*>(g);
        }
        else if(algo == "Kruskal"){
            PipelineKruskal* g = Singletone<PipelineKruskal>::getInstance();
            return static_cast<Pipeline*>(g);
        }
        else{
            throw invalid_argument("Algorithm not supported!");
        }
    }
};