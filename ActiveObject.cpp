#include "ActiveObject.hpp"

ActiveObject::ActiveObject(Graph (*process)(Graph*),ActiveObject* next){
    _tasks = queue<Graph*>();
    _cond = PTHREAD_COND_INITIALIZER;
    _mutex = PTHREAD_MUTEX_INITIALIZER;
    _process = process;
    _next = next;
}

void ActiveObject::run(){
    pthread_mutex_lock(&_mutex);
    while (_tasks.empty()) {
        pthread_cond_wait(&_cond, &_mutex); // Wait for the queue to have an item
    }
    Graph* task = _tasks.front();
    cout << __func__ <<" Handling new task: " << task <<endl;
    _tasks.pop();
    Graph result = _process(task);
    pthread_mutex_unlock(&_mutex);
    if(_next){
        _next->pushTask(&result);
    }
}

void ActiveObject::pushTask(Graph* task){
    pthread_mutex_lock(&_mutex);
    _tasks.push(task);
    pthread_mutex_unlock(&_mutex);
    pthread_cond_signal(&_cond);
}

ActiveObject*  ActiveObject::getNext(){
    return _next;
}
