#include "LeaderFollower.hpp"
#include <unistd.h>
#include <iostream>

// Constructor
LeaderFollower::LeaderFollower(pair<int, Graph> (*mstAlgo)(int, Graph), int threadCount, int queueLimit)
    : _mstAlgo(mstAlgo),
      _threadCount(threadCount),
      _queueLimit(queueLimit),
      _isRunning(false),
      _isStarted(false) {
    pthread_mutex_init(&_taskMutex, nullptr);
    pthread_cond_init(&_taskCond, nullptr);
    pthread_cond_init(&_queueCond, nullptr);
    pthread_cond_init(&_leaderCond, nullptr);
    _leaderThread = 0;
}

// Destructor
LeaderFollower::~LeaderFollower() {
    stop();
    pthread_mutex_destroy(&_taskMutex);
    pthread_cond_destroy(&_taskCond);
    pthread_cond_destroy(&_queueCond);
    pthread_cond_destroy(&_leaderCond);
}

// Start the thread pool
void LeaderFollower::start() {
    if (_isStarted) return;  // Prevent multiple starts
    _isStarted = true;
    _isRunning = true;

    for (int i = 0; i < _threadCount; ++i) {
        pthread_t thread;
        pthread_create(&thread, nullptr, workerThread, this);
        _threads.push_back(thread);
    }
    _leaderThread = _threads[0];
}

// Stop the thread pool
void LeaderFollower::stop() {
    pthread_mutex_lock(&_taskMutex);
    _isRunning = false;
    pthread_cond_broadcast(&_taskCond);  // Wake all waiting threads
    pthread_cond_broadcast(&_leaderCond);
    pthread_mutex_unlock(&_taskMutex);

    for (pthread_t thread : _threads) {
        if(pthread_join(thread, nullptr)!=0){
            std::cerr << "Failed to join thread" << std::endl;
        }
    }
    _threads.clear();
    _isStarted = false;
}

// Add a task to the queue
void LeaderFollower::addTask(int fd, Graph graph) {
    pthread_mutex_lock(&_taskMutex);
    while (_tasks.size() >= _queueLimit) {
        pthread_cond_wait(&_queueCond, &_taskMutex);  // Wait for space
    }
    _tasks.push({fd, graph});
    pthread_cond_signal(&_taskCond);  // Signal a worker
    pthread_mutex_unlock(&_taskMutex);
}

// Worker thread function
void* LeaderFollower::workerThread(void* arg) {
    LeaderFollower* lf = static_cast<LeaderFollower*>(arg);

    while (true) {
        pthread_mutex_lock(&lf->_taskMutex);

        while (lf->_isRunning && lf->_tasks.empty()) {
            pthread_cond_wait(&lf->_taskCond, &lf->_taskMutex);
        }

        if (!lf->_isRunning && lf->_tasks.empty()) {
            pthread_mutex_unlock(&lf->_taskMutex);
            break;
        }

        // Leader transition logic
        if (lf->_leaderThread == pthread_self()) {
            auto it = std::find_if(
                lf->_threads.begin(),
                lf->_threads.end(),
                [](pthread_t t) { return pthread_equal(t, pthread_self()); });

            if (it != lf->_threads.end() && std::next(it) != lf->_threads.end()) {
                lf->_leaderThread = *std::next(it);
            } else {
                lf->_leaderThread = lf->_threads[0];
            }
            pthread_cond_signal(&lf->_leaderCond);
        }

        // Process task
        auto task = lf->_tasks.front();
        lf->_tasks.pop();
        pthread_mutex_unlock(&lf->_taskMutex);

        try {
            auto result = lf->_mstAlgo(task.first, task.second);
            result = GraphAlgo::getTotalWeight(result.first, result.second);
            result = DistanceAlgo::FloydWarshall(result.first, result.second);
            result = GraphAlgo::averageDistance(result.first, result.second);
            result = GraphAlgo::longestDistance(result.first, result.second);
            result = GraphAlgo::shortestDistance(result.first, result.second);

        } catch (const std::exception& e) {
            std::cerr << "Exception in task processing: " << e.what() << std::endl;
        }
    }
    return nullptr;
}
