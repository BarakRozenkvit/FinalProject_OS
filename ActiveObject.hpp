#include <queue>
#include <mutex>
#include <condition_variable>
#include <pthread.h>
#include "Graph.hpp"
#pragma once

using namespace std;

class ActiveObject{
    queue<pair<int,Graph>> _tasks;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    pair<int,Graph> (*_process)(int,Graph);
    ActiveObject* _next;

public:
    ActiveObject(pair<int,Graph> (*process)(int,Graph),ActiveObject* next);
    void run();
    void pushTask(pair<int,Graph> task);
    ActiveObject* getNext();
};