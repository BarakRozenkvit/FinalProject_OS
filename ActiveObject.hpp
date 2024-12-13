#include <queue>
#include <mutex>
#include <condition_variable>
#include <pthread.h>
#include "Graph.hpp"
#pragma once

using namespace std;

class ActiveObject{
    queue<Graph*> _tasks;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    Graph (*_process)(Graph*);
    ActiveObject* _next;

public:
    ActiveObject(Graph (*process)(Graph*),ActiveObject* next);
    void run();
    void pushTask(Graph* task);
    ActiveObject* getNext();
};