#include <vector>
#include "ActiveObject.hpp"

// class Pipeline{




// }


class PipelinePrim{

    // Pipeline order of Active Objects
    vector<ActiveObject*> _pipeline;

public:
    PipelinePrim(){

        
    }

        // Starting all active objects in diffrent thread
        execute();
        // getting new task to pipeline
        enqueue_new_task();


} 