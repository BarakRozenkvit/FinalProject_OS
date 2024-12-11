#include <queue>
#include <mutex>
#include <condition_variable>
#include <pthread.h>
#include "Graph.hpp"
#include "Tree.hpp"
#include "Algo.hpp"

using namespace std;

template <typename Incoming, typename Outgoing>
class ActiveObject{
protected:
    queue<Incoming> _tasks;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    Outgoing(*_process)(Incoming)
    void* _next;

public:
    ActiveObject(Outgoing(*process)(Incoming),void* next);
    void run();
    void getTask(Incoming task);
    void forwardTask(Outgoing task);
    void* nextStage(){
        return _next;
    }
};