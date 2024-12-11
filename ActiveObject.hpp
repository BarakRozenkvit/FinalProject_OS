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

    queue<Incoming> _tasks;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    ActiveObject* _next;

public:
    ActiveObject(ActiveObject* next);
    virtual void execute() =0;
    void getTask(Incoming task);
    void forwardTask(Outgoing task);
};

class Kruskal: public ActiveObject<Graph,Tree>{
public:
    void execute() override;
};

class Prim: public ActiveObject<Graph,Tree>{
public:
    void execute() override;
};