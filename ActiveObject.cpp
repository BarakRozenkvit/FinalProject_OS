#include "ActiveObject.hpp"
#include <unistd.h>

ActiveObject::ActiveObject(pair<int,Graph> (*process)(int,Graph),ActiveObject* next){
    _tasks = queue<pair<int,Graph>>();
    _cond = PTHREAD_COND_INITIALIZER;
    _mutex = PTHREAD_MUTEX_INITIALIZER;
    _process = process;
    _next = next;
}

void ActiveObject::run(){
    while(1){
        pthread_mutex_lock(&_mutex);
        bool tasksEmpty = _tasks.empty();
        pthread_mutex_unlock(&_mutex);
    
        if (tasksEmpty){
            pthread_cond_wait(&_cond, &_mutex); // Wait for the queue to have an item
        }
        
        pair<int,Graph> task = _tasks.front();
        _tasks.pop();
        sleep(0.5);
        pair<int,Graph> result = _process(task.first,task.second);
        if(_next){
            _next->pushTask(result);
        }
        pthread_mutex_unlock(&_mutex);
    }
}

void ActiveObject::pushTask(pair<int,Graph> task){
    pthread_mutex_lock(&_mutex);
    _tasks.push(task);
    pthread_cond_signal(&_cond);
    pthread_mutex_unlock(&_mutex);
}

ActiveObject*  ActiveObject::getNext(){
    return _next;
}
