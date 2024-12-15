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
        // lock the mutex to know if the queue is empty
        pthread_mutex_lock(&_mutex);
        bool tasksEmpty = _tasks.empty();
        pthread_mutex_unlock(&_mutex);
    
        if (tasksEmpty){
            // if queue is empty wait for cond that new task cames
            pthread_cond_wait(&_cond, &_mutex);
        }
        
        // get new task and pop queue
        pair<int,Graph> task = _tasks.front();
        _tasks.pop();
        sleep(0.5);
        // get result from process
        pair<int,Graph> result = _process(task.first,task.second);
        // if next active object push task to its queue
        if(_next){
            _next->pushTask(result);
        }
        // unlock the mutex
        pthread_mutex_unlock(&_mutex);
    }
}

void ActiveObject::pushTask(pair<int,Graph> task){
    // lock the mutex for pushing the task to my queue
    pthread_mutex_lock(&_mutex);
    _tasks.push(task);
    // signal that a new task came
    pthread_cond_signal(&_cond);
    // unlock the mutex
    pthread_mutex_unlock(&_mutex);
}

ActiveObject*  ActiveObject::getNext(){
    return _next;
}
