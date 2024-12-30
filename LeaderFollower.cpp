// #include "LeaderFollower.hpp"
// #include <unistd.h>
// #include <iostream>

// /**
//  * Constructor initializes thread pool resources
//  * @param mstAlgo Function pointer to MST algorithm
//  * @param threadCount Number of threads in pool
//  */
// LeaderFollower::LeaderFollower(pair<int, Graph> (*mstAlgo)(int, Graph), int threadCount, int _queueLimit)
//     : _threadCount(threadCount), _isRunning(false), _mstAlgo(mstAlgo), _queueLimit(_queueLimit) {
//     pthread_mutex_init(&_taskMutex, nullptr);
//     pthread_cond_init(&_taskCond, nullptr);
//     pthread_cond_init(&_queueCond, nullptr);
//     pthread_cond_init(&_leaderCond, nullptr);  // Initialize leader condition
//     _leaderThread = 0;                         // Initially no leader
// }

// /**
//  * Destructor ensures clean shutdown and resource cleanup
//  */
// LeaderFollower::~LeaderFollower() {
//     if (_isRunning) {
//         stop();
//     }
//     // Ensure no threads are using mutexes before destroying
//     pthread_mutex_destroy(&_taskMutex);
//     pthread_cond_destroy(&_taskCond);
//     pthread_cond_destroy(&_queueCond);
//     pthread_cond_destroy(&_leaderCond);
// }

// /**
//  * Start the thread pool
//  * Creates worker threads and begins task processing
//  */
// void LeaderFollower::start() {
//     _isRunning = true;
//     // Create worker threads
//     for (int i = 0; i < _threadCount; i++) {
//         pthread_t thread;
//         // Create threads that will execute the workerThread() func.
//         pthread_create(&thread, nullptr, workerThread, this);
//         _threads.push_back(thread);
//     }
//         _leaderThread = _threads[0];
//     std::cout << "Init leader thread: " << _leaderThread << std::endl;
// }

// void LeaderFollower::destroyAll() {
//     Singletone<LeaderFollowerPrim>::destroyInstance();
//     Singletone<LeaderFollowerKruskal>::destroyInstance();
// }

// /**
//  * Stop the thread pool
//  * Signals all threads to stop and waits for completion
//  */
// void LeaderFollower::stop() {
//     pthread_mutex_lock(&_taskMutex);
//     _isRunning = false;
//     pthread_cond_broadcast(&_taskCond);    // Wake all waiting threads
//     pthread_cond_broadcast(&_leaderCond);  // Wake threads waiting for leadership
//     pthread_mutex_unlock(&_taskMutex);

//     // Wait for all threads to finish
//     for (pthread_t thread : _threads) {
//         pthread_join(thread, nullptr);
//     }
//     _threads.clear();
// }

// /**
//  * Add new MST calculation task to queue
//  * @param fd Client file descriptor
//  * @param graph Graph to process
//  */
// void LeaderFollower::addTask(int fd, Graph graph) {
//     pthread_mutex_lock(&_taskMutex);  // rotects _tasks while checking and modifying its size.
//                                       // Prevents race conditions when multiple threads access _tasks.
//     while (_tasks.size() >= _queueLimit) {
//         pthread_cond_wait(&_queueCond, &_taskMutex);  // If queue is full, releases lock and waits for _queueCond.
//                                                       // When signaled, it re-acquires the lock to proceed.
//     }
//     _tasks.push({fd, graph});
//     pthread_cond_signal(&_taskCond);    //  Notifies a waiting worker thread that a new task is available.
//     pthread_mutex_unlock(&_taskMutex);  // Releases the lock after modifying _tasks, allowing other threads to access _tasks.
// }

// void* LeaderFollower::workerThread(void* arg) {
//     // Cast the argument to LeaderFollower instance
//     LeaderFollower* lf = static_cast<LeaderFollower*>(arg);

//     while (true) {
//         // Lock the task mutex before checking for tasks
//         pthread_mutex_lock(&lf->_taskMutex);

//         // Wait until a task is available or the system is stopped
//         while (lf->_isRunning && lf->_tasks.empty()) {
//             pthread_cond_wait(&lf->_taskCond, &lf->_taskMutex);
//         }

//         // Exit condition: stop if system is stopped and no tasks are available
//         if (!lf->_isRunning && lf->_tasks.empty()) {
//             pthread_mutex_unlock(&lf->_taskMutex);
//             break;
//         }

//         // Transition leadership if current thread is leader
//         if (lf->_leaderThread == pthread_self()) {
//             std::cout << "Leader thread: " <<  pthread_self() << std::endl;

//             // Find the current leader in the thread vector using pthread_equal
//             auto it = std::find_if(
//                 lf->_threads.begin(),
//                 lf->_threads.end(),
//                 [](pthread_t t) { return pthread_equal(t, pthread_self()); }
//             );

//              // Update leader to the next thread in the pool
//             if (it != lf->_threads.end() && std::next(it) != lf->_threads.end()) {
//                 lf->_leaderThread = *std::next(it);
//             } else {
//                 lf->_leaderThread = lf->_threads[0];  // Wrap around to the first thread
//             }

//             std::cout << "New leader thread: " << lf->_leaderThread << std::endl;

//             pthread_cond_signal(&lf->_leaderCond);  // Signal next leader
//         }

//         // Process task
//         auto task = lf->_tasks.front();
//         lf->_tasks.pop();
//         pthread_mutex_unlock(&lf->_taskMutex);

//         // Execute the MST task
//         try {
//             pair<int, Graph> result = lf->_mstAlgo(task.first, task.second);
//             result = GraphAlgo::getTotalWeight(result.first, result.second);
//             result = DistanceAlgo::FloydWarshall(result.first, result.second);
//             result = GraphAlgo::averageDistance(result.first, result.second);
//             result = GraphAlgo::longestDistance(result.first, result.second);
//             result = GraphAlgo::shortestDistance(result.first, result.second);
//         } catch (const std::exception& e) {
//             std::cerr << "Task exception: " << e.what() << std::endl;
//         }
//     }

//     return nullptr;
// }
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
    // std::cout << "[LeaderFollower] Initial leader thread: " << _leaderThread << std::endl;
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
            cout << "[LeaderFollower] New leader thread: " << lf->_leaderThread << endl;
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

            std::cout << "[LeaderFollower] Task completed for client " << task.first << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[LeaderFollower] Exception while processing task: " << e.what() << std::endl;
        }
    }
    return nullptr;
}
