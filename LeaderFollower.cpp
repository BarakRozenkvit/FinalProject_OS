#include "LeaderFollower.hpp"
#include <unistd.h>
#include <iostream>


/**
 * Constructor initializes thread pool resources
 * @param mstAlgo Function pointer to MST algorithm
 * @param threadCount Number of threads in pool
 */
LeaderFollower::LeaderFollower(pair<int, Graph> (*mstAlgo)(int, Graph), int threadCount, int _queueLimit)
    : _threadCount(threadCount), _isRunning(false), _mstAlgo(mstAlgo), _queueLimit(_queueLimit) {
    pthread_mutex_init(&_taskMutex, nullptr);
    pthread_cond_init(&_taskCond, nullptr);
    pthread_cond_init(&_queueCond, nullptr);
    pthread_cond_init(&_leaderCond, nullptr);  // Initialize leader condition
    _leaderThread = 0;                         // Initially no leader
}

/**
 * Destructor ensures clean shutdown and resource cleanup
 */
LeaderFollower::~LeaderFollower() {
    if (_isRunning) {
        stop();
    }
    // Ensure no threads are using mutexes before destroying
    pthread_mutex_destroy(&_taskMutex);
    pthread_cond_destroy(&_taskCond);
    pthread_cond_destroy(&_queueCond);
    pthread_cond_destroy(&_leaderCond);
}

/**
 * Start the thread pool
 * Creates worker threads and begins task processing
 */
void LeaderFollower::start() {
    _isRunning = true;
    // Create worker threads
    for (int i = 0; i < _threadCount; i++) {
        pthread_t thread;
        // Create threads that will execute the workerThread() func.
        pthread_create(&thread, nullptr, workerThread, this);
        _threads.push_back(thread);
    }
}

void LeaderFollower::destroyAll() {
    Singletone<LeaderFollowerPrim>::destroyInstance();
    Singletone<LeaderFollowerKruskal>::destroyInstance();
}


/**
 * Stop the thread pool
 * Signals all threads to stop and waits for completion
 */
void LeaderFollower::stop() {
    pthread_mutex_lock(&_taskMutex);
    _isRunning = false;
    pthread_cond_broadcast(&_taskCond);  // Wake all waiting threads
    pthread_cond_broadcast(&_leaderCond); // Wake threads waiting for leadership
    pthread_mutex_unlock(&_taskMutex);

    // Wait for all threads to finish
    for (pthread_t thread : _threads) {
        pthread_join(thread, nullptr);
    }
    _threads.clear();
}

/**
 * Add new MST calculation task to queue
 * @param fd Client file descriptor
 * @param graph Graph to process
 */
void LeaderFollower::addTask(int fd, Graph graph) {
    pthread_mutex_lock(&_taskMutex); // rotects _tasks while checking and modifying its size.
                                     // Prevents race conditions when multiple threads access _tasks.
    while (_tasks.size() >= _queueLimit) {
        pthread_cond_wait(&_queueCond, &_taskMutex);  // If queue is full, releases lock and waits for _queueCond. 
                                                      // When signaled, it re-acquires the lock to proceed.
    }
    _tasks.push({fd, graph});
    pthread_cond_signal(&_taskCond);  //  Notifies a waiting worker thread that a new task is available.
    pthread_mutex_unlock(&_taskMutex); // Releases the lock after modifying _tasks, allowing other threads to access _tasks.
}

/**
 * Worker thread function
 * Continuously processes tasks from queue until shutdown
 */
void* LeaderFollower::workerThread(void* arg) {
    LeaderFollower* lf = static_cast<LeaderFollower*>(arg);

    while (true) {
        pthread_mutex_lock(&lf->_taskMutex);
        
        if (!lf->_isRunning && lf->_tasks.empty()) {
            pthread_mutex_unlock(&lf->_taskMutex);
            break;
        }

        // Wait for tasks
        while (lf->_tasks.empty() && lf->_isRunning) {
            pthread_cond_wait(&lf->_taskCond, &lf->_taskMutex);
        }

        // // Recheck condition after wait
        // if (!lf->_isRunning && lf->_tasks.empty()) {
        //     pthread_mutex_unlock(&lf->_taskMutex);
        //     break;
        // }

        // Process task if available
        if (!lf->_tasks.empty()) {
            auto task = lf->_tasks.front();
            lf->_tasks.pop();
            pthread_mutex_unlock(&lf->_taskMutex);
            
            // Process task
            try {
                pair<int, Graph> result = lf->_mstAlgo(task.first, task.second);
                result = DistanceAlgo::FloydWarshall(result.first, result.second);
                result = GraphAlgo::getTotalWeight(result.first, result.second);
                result = GraphAlgo::averageDistance(result.first, result.second);
                result = GraphAlgo::longestDistance(result.first, result.second);
                result = GraphAlgo::shortestDistance(result.first, result.second);
            } catch (const std::exception& e) {
                std::cerr << "Exception during task processing: " << e.what() << std::endl;
            }
        } else {
            pthread_mutex_unlock(&lf->_taskMutex);
        }
    }
    return nullptr;
}

/**
 * Process next task from queue
 * Calculates MST and additional metrics
 */
void LeaderFollower::processNextTask() {
    pthread_mutex_lock(&_taskMutex);
    if (_tasks.empty()) {
        pthread_mutex_unlock(&_taskMutex);
        return;
    }

    // Get next task from queue
    auto task = _tasks.front();
    _tasks.pop();
    pthread_mutex_unlock(&_taskMutex);
    // Release mutex during long calculation

    try {
        pair<int, Graph> result = _mstAlgo(task.first, task.second);
        result = DistanceAlgo::FloydWarshall(result.first, result.second);
        result = GraphAlgo::getTotalWeight(result.first, result.second);
        result = GraphAlgo::averageDistance(result.first, result.second);
        result = GraphAlgo::longestDistance(result.first, result.second);
        result = GraphAlgo::shortestDistance(result.first, result.second);
    } catch (const std::exception& e) {
        std::cerr << "Exception during task processing: " << e.what() << std::endl;
    }

    pthread_mutex_lock(&_taskMutex);  // Relock after processing
    pthread_cond_signal(&_taskCond);
    pthread_mutex_unlock(&_taskMutex);
}

/**
 * Log the current status of the task queue and threads
 */
void LeaderFollower::logStatus() {
    pthread_mutex_lock(&_taskMutex);
    std::cerr << "Thread Pool Status: " << _threads.size()
              << " threads, Queue Size: " << _tasks.size()
              << ", Queue Limit: " << _queueLimit << std::endl;
    pthread_mutex_unlock(&_taskMutex);
}