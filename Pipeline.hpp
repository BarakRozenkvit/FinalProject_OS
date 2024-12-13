#include <vector>
#include <pthread.h>
#include "ActiveObject.hpp"
#include "Algo.hpp"
#include "Singletone.hpp"
#pragma once

class Pipeline{

    ActiveObject* _stage;
    
public:
    Pipeline(Graph (*mstAlgo) (Graph*));
    ~Pipeline();

    void execute();

    void addTask(Graph* graph);

    static void* runStage(void* stage);

    void getNext(){
        cout << _stage << endl;
    }
};

class PipelinePrim: public Pipeline{

public:
    PipelinePrim(): Pipeline(MSTAlgo::Prim){};
    ~PipelinePrim() = default;
};

class PipelineKruskal: public Pipeline{

public:
    PipelineKruskal(): Pipeline(MSTAlgo::Kruskal){};
    ~PipelineKruskal() = default;
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