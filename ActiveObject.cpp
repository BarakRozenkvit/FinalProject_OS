#include "ActiveObject.hpp"

template <typename Incoming, typename Outgoing>
ActiveObject<Incoming,Outgoing>::ActiveObject(ActiveObject* next){
    _tasks = queue<Incoming>();
    _cond = PTHREAD_COND_INITIALIZER;
    _mutex = PTHREAD_MUTEX_INITIALIZER;
    _next = next;
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

