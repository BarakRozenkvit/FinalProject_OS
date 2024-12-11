#include "ActiveObject.hpp"

template <typename Incoming, typename Outgoing>
ActiveObject<Incoming,Outgoing>::ActiveObject(Outgoing(*process)(Incoming),void* next){
    _tasks = queue<Incoming>();
    _cond = PTHREAD_COND_INITIALIZER;
    _mutex = PTHREAD_MUTEX_INITIALIZER;
    _process(process);
    _next = next;
}

template <typename Incoming, typename Outgoing>
void ActiveObject<Incoming, Outgoing>::run(){
    pthread_mutex_lock(&_mutex);
    while (_tasks.empty()) {
        pthread_cond_wait(&cond, &mtx); // Wait for the queue to have an item
    }
    Incoming item = _tasks.front();
    _tasks.pop();
    Outgoing task = _process(item);
    pthread_mutex_unlock(&mtx);
    _next.forwardTask(task);
}

template <typename Incoming, typename Outgoing>
void ActiveObject<Incoming, Outgoing>::getTask(Incoming task){
    pthread_mutex_lock(&_mutex);
    _tasks.push(task);
    pthread_mutex_unlock(&_mutex);
    pthread_cond_signal(&_cond);
}

template <typename Incoming, typename Outgoing>
void ActiveObject<Incoming, Outgoing>::forwardTask(Outgoing task){
    _next->getTask(task);
}



