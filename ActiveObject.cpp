#include "ActiveObject.hpp"
#include <unistd.h>

pthread_mutex_t ActiveObject::isRunningMutex=PTHREAD_MUTEX_INITIALIZER;
bool ActiveObject::isRunning = true;

ActiveObject::ActiveObject(pair<int,Graph> (*process)(int,Graph),ActiveObject* next){
    _tasks = queue<pair<int,Graph>>();
    _cond = PTHREAD_COND_INITIALIZER;
    _mutex = PTHREAD_MUTEX_INITIALIZER;
    _process = process;
    _next = next;
}

ActiveObject::~ActiveObject(){}


void ActiveObject::run() {
    while (true) {

        pthread_mutex_lock(&_mutex);
        while (_tasks.empty()) {
            pthread_cond_wait(&_cond, &_mutex); // Automatically releases the mutex while waiting
            pthread_mutex_lock(&isRunningMutex);
            bool run = isRunning;
            pthread_mutex_unlock(&isRunningMutex);
            if (!run){
                pthread_mutex_unlock(&_mutex);
                return;
            }
        }
        // Get new task and pop from the queue
        auto task = _tasks.front();
        _tasks.pop();

        pthread_mutex_unlock(&_mutex); // Unlock the mutex before processing the task

        // Process the task
        try {
            auto result = _process(task.first, task.second);

            // Push the result to the next active object if it exists
            if (_next) {
                _next->pushTask(result);
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception in task processing: " << e.what() << std::endl;
        }
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

void ActiveObject::signalStop(){
    pthread_mutex_lock(&_mutex);
    pthread_cond_signal(&_cond);
    pthread_mutex_unlock(&_mutex);
}
