#include <queue>
#include <mutex>
#include <condition_variable>
#include <pthread.h>
#include "Graph.hpp"
#pragma once

using namespace std;

class ActiveObject{
    
    // Queue of tasks with pair of clients fd and requests graph to caculate
    queue<pair<int,Graph>> _tasks;
    // mutex for active object
    pthread_mutex_t _mutex;
    // cond for active object
    pthread_cond_t _cond;
    // process each active object holds
    pair<int,Graph> (*_process)(int,Graph);
    // next active object to push tasks
    ActiveObject* _next;
    // 
    bool _isRunning;

public:
    // Active object gets a function that he runs and the next active object
    ActiveObject(pair<int,Graph> (*process)(int,Graph),ActiveObject* next);
    // destory active object
    ~ActiveObject();
    // run function that is inside thread
    void run();
    // push task to next active object
    void pushTask(pair<int,Graph> task);
    // get next object
    ActiveObject* getNext();
};