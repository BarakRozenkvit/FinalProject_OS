#include "LeaderFollower.hpp"
#include "Util.hpp"
#include <unistd.h>

// Initialize static members
LeaderFollower* LeaderFollowerFactory::primLF = nullptr;
LeaderFollower* LeaderFollowerFactory::kruskalLF = nullptr;

LeaderFollower::LeaderFollower(pair<int,Graph> (*mstAlgo)(int, Graph), int threadCount)
    : _threadCount(threadCount), _isRunning(false), _mstAlgo(mstAlgo) {
    pthread_mutex_init(&_taskMutex, nullptr);
    pthread_cond_init(&_taskCond, nullptr);
}

LeaderFollower::~LeaderFollower() {
    stop();
    pthread_mutex_destroy(&_taskMutex);
    pthread_cond_destroy(&_taskCond);
}

void LeaderFollower::start() {
    _isRunning = true;
    // Create worker threads
    for (int i = 0; i < _threadCount; i++) {
        pthread_t thread;
        pthread_create(&thread, nullptr, workerThread, this);
        _threads.push_back(thread);
    }
}

void LeaderFollower::stop() {
    _isRunning = false;
    pthread_cond_broadcast(&_taskCond);
    
    for (pthread_t thread : _threads) {
        pthread_join(thread, nullptr);
    }
    _threads.clear();
}

void LeaderFollower::addTask(int fd, Graph graph) {
    pthread_mutex_lock(&_taskMutex);
    _tasks.push(make_pair(fd, graph));
    pthread_cond_signal(&_taskCond);  // Wake up one worker
    pthread_mutex_unlock(&_taskMutex);
}

void* LeaderFollower::workerThread(void* arg) {
    LeaderFollower* lf = static_cast<LeaderFollower*>(arg);
    
    while (lf->_isRunning) {
        pthread_mutex_lock(&lf->_taskMutex);
        
        while (lf->_tasks.empty() && lf->_isRunning) {
            pthread_cond_wait(&lf->_taskCond, &lf->_taskMutex);
        }
        
        if (!lf->_isRunning) {
            pthread_mutex_unlock(&lf->_taskMutex);
            break;
        }
        
        lf->processNextTask();
        pthread_mutex_unlock(&lf->_taskMutex);
    }
    
    return nullptr;
}

void LeaderFollower::processNextTask() {
    if (_tasks.empty()) return;
    
    // Get next task
    auto task = _tasks.front();
    _tasks.pop();
    
    // Release mutex while processing
    pthread_mutex_unlock(&_taskMutex);
    
    // Process MST calculation
    pair<int, Graph> result = _mstAlgo(task.first, task.second);
    
    // Calculate additional metrics like in Pipeline
    result = GraphAlgo::getTotalWeight(result.first, result.second);
    result = DistanceAlgo::FloyedWarshall(result.first, result.second);
    result = GraphAlgo::averageDistance(result.first, result.second);
    result = GraphAlgo::longestDistance(result.first, result.second);
    result = GraphAlgo::shortestDistance(result.first, result.second);
    
    // Reacquire mutex for next task
    pthread_mutex_lock(&_taskMutex);
} 