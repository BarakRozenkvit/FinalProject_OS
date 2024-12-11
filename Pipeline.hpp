#include <vector>
#include "ActiveObject.hpp"
#include "Graph.hpp"
#include "Tree.hpp"

// class Pipeline{




// }


class PipelinePrim{

    void* _pipline;

public:
    PipelinePrim(){
        _pipline = new ActiveObject<Graph,Tree>(MSTAlgo::Prim,nullptr);
    }

    void build(){
        
        _pipline = new ActiveObject<Graph,Tree>(MSTAlgo::Prim,nullptr);
    }
    void execute(){

        while(_pipline->nextStage()!=nullptr){

        }
    }
        // getting new task to pipeline
        enqueue_new_task();


} 