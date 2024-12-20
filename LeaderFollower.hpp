#pragma once
#include <pthread.h>
#include "Proactor.hpp"
#include <queue>
#include <vector>
#include "Algo.hpp"
#include "Graph.hpp"

/**
 * LeaderFollower class implements a thread pool pattern for MST calculations
 * Each thread can process MST tasks from a shared queue
 */
class LeaderFollower {
   private:
    std::queue<std::pair<int, Graph>> _tasks;  // Queue holds pairs of (client_fd, graph)
    std::vector<pthread_t> _threads;           // Pool of worker threads
    int _queueLimit;                           // Queue size limit
    pthread_mutex_t _taskMutex;                // Mutex for protecting task queue
    pthread_t _leaderThread;                   // Thread ID of the current leader
    pthread_cond_t _leaderCond;                // Condition variable to signal leadership transfer
    pthread_cond_t _queueCond;                 // Condition variable to signal when space is available
    pthread_cond_t _taskCond;                  // Condition variable for task availability
    bool _isRunning;                           // Flag for thread pool status
    int _threadCount;                          // Number of threads in pool
    pair<int, Graph> (*_mstAlgo)(int, Graph);  // Function pointer to MST algorithm (Prim/Kruskal)

   public:
    // Constructor initializes thread pool with specified algorithm and thread count
    LeaderFollower(pair<int, Graph> (*mstAlgo)(int, Graph), int threadCount = 5, int _queueLimi = 10);
    ~LeaderFollower();

    // Start the thread pool
    void start();
    // Stop the thread pool and cleanup
    void stop();
    // Add new MST calculation task to queue
    void addTask(int fd, Graph graph);
    // Log status for inpecting threads and task queue.
    void logStatus();

    static void destroyAll();

   private:
    // Static function for thread execution
    static void* workerThread(void* arg);
    // Process next task from queue
    void processNextTask();
};



class LeaderFollowerPrim : public LeaderFollower {
   public:
    LeaderFollowerPrim(int threadCount = 5, int _queueLimit = 10) : LeaderFollower(MSTAlgo::Prim, threadCount, _queueLimit) {}
};

class LeaderFollowerKruskal : public LeaderFollower {
    public :
     LeaderFollowerKruskal(int threadCount = 5, int _queueLimit = 10) : LeaderFollower(MSTAlgo::Kruskal, threadCount, _queueLimit) {}
};


/**
 * Factory class for creating and managing LeaderFollower instances
 * Uses singleton pattern to maintain one instance per algorithm type
 */
class LeaderFollowerFactory {

   public:
    /**
     * Get or create LeaderFollower instance for specified algorithm
     * @param algo Algorithm name ("Prim" or "Kruskal")
     * @return Pointer to LeaderFollower instance
     * @throws invalid_argument if algorithm not supported
     */
    static LeaderFollower* get(string algo) {
        if (algo == "Prim") {
            LeaderFollowerPrim* LFP = Singletone<LeaderFollowerPrim>::getInstance();
            LFP->start();
            return LFP;
        }

        else if (algo == "Kruskal") {
                LeaderFollowerKruskal* LFK = Singletone<LeaderFollowerKruskal>::getInstance();
                LFK->start();
                return LFK;
            }
        throw std::invalid_argument("Invalid algorithm");
        return nullptr;
    }

};