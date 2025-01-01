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

// Destructor: Cleans up resources by stopping the thread pool and destroying synchronization primitives.
LeaderFollower::~LeaderFollower() {
    stop(); // Ensure all threads are stopped
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

    // Create threads and store their IDs in the _threads vector
    // would help manage the leader switching logic.
    for (int i = 0; i < _threadCount; ++i) {
        pthread_t thread;
        // Creates _threadCount threads that will execute the workerThread function
        // and pass the current LeaderFollower instance as an argument.
        pthread_create(&thread, nullptr, workerThread, this);
        _threads.push_back(thread);
    }
    _leaderThread = _threads[0]; // Set the first thread as the leader
}

// Stop the thread pool and join all threads.
void LeaderFollower::stop() {
    pthread_mutex_lock(&_taskMutex);
    _isRunning = false;
    pthread_cond_broadcast(&_taskCond);  // Wake all threads waiting for tasks
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


// Worker thread function that each thread executes.
// Handles task execution and leader transitions.
/*

@param arg: Pointer to the LeaderFollower instance
*/
void* LeaderFollower::workerThread(void* arg) {
    LeaderFollower* lf = static_cast<LeaderFollower*>(arg);

    while (true) {
        pthread_mutex_lock(&lf->_taskMutex);

        // if there are no tasks, wait for a signal
        while (lf->_isRunning && lf->_tasks.empty()) {
            pthread_cond_wait(&lf->_taskCond, &lf->_taskMutex);
        }

        // Exit if thread pool is stopped and no tasks are available
        if (!lf->_isRunning && lf->_tasks.empty()) {
            pthread_mutex_unlock(&lf->_taskMutex);
            break;
        }

        // Leader transition logic: The leader assigns a new leader before processing its task
        if (lf->_leaderThread == pthread_self()) {

            // Find the current thread in the list 
            // and set the next thread as the leader
            auto it = std::find_if(
                lf->_threads.begin(),
                lf->_threads.end(),
                [](pthread_t t) { //
                    return pthread_equal(t, pthread_self());});

            if (it != lf->_threads.end() && std::next(it) != lf->_threads.end()) {
                // Set the next thread as the new leader
                lf->_leaderThread = *std::next(it);
            } else {
                // If the current thread is the last thread, 
                // start over and set the first thread as the new leader.
                lf->_leaderThread = lf->_threads[0];
            }
            // Signal the new leader
            pthread_cond_signal(&lf->_leaderCond);
        }

        // Process the next task in the queue
        auto task = lf->_tasks.front();
        lf->_tasks.pop();
        pthread_mutex_unlock(&lf->_taskMutex);

        try {
            // Execute the relevant mstAlgo for each instance of LeaderFollower.
            auto result = lf->_mstAlgo(task.first, task.second);
            // Perform additional functions...
            result = GraphAlgo::getTotalWeight(result.first, result.second);
            result = DistanceAlgo::FloydWarshall(result.first, result.second);
            result = GraphAlgo::averageDistance(result.first, result.second);
            result = GraphAlgo::longestDistance(result.first, result.second);
            result = GraphAlgo::shortestDistance(result.first, result.second);

        } catch (const std::exception& e) {
            // Handle task-specific exceptions gracefully
            std::cerr << "Exception in task processing: " << e.what() << std::endl;
        }
    }
    return nullptr;
}

// Add a task to the queue
void LeaderFollower::addTask(int fd, Graph graph) {
    pthread_mutex_lock(&_taskMutex);

    while (_tasks.size() >= _queueLimit) { // in case of full queue
        pthread_cond_wait(&_queueCond, &_taskMutex);  // Wait for space
    }

    _tasks.push({fd, graph});   // Add task to the queue
    pthread_cond_signal(&_taskCond);  // Signal a thread to process the task
    pthread_mutex_unlock(&_taskMutex);
}