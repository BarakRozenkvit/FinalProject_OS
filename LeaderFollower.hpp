#pragma once
#include <queue>
#include <pthread.h>
#include <vector>
#include "Graph.hpp"
#include "Algo.hpp"

class LeaderFollower {
private:
    std::queue<std::pair<int, Graph>> _tasks;  // Queue of (fd, graph) pairs
    std::vector<pthread_t> _threads;
    pthread_mutex_t _taskMutex;
    pthread_cond_t _taskCond;
    bool _isRunning;
    int _threadCount;
    pair<int,Graph> (*_mstAlgo)(int, Graph);  // MST algorithm to use (Prim or Kruskal)

public:
    LeaderFollower(pair<int,Graph> (*mstAlgo)(int, Graph), int threadCount = 5);
    ~LeaderFollower();
    
    void start();
    void stop();
    void addTask(int fd, Graph graph);

private:
    static void* workerThread(void* arg);
    void processNextTask();
};

// Singleton factory for LeaderFollower instances
class LeaderFollowerFactory {
    static LeaderFollower* primLF;
    static LeaderFollower* kruskalLF;

public:
    static LeaderFollower* get(string algo) {
        if (algo == "Prim") {
            if (!primLF) {
                primLF = new LeaderFollower(MSTAlgo::Prim);
                primLF->start();
            }
            return primLF;
        }
        else if (algo == "Kruskal") {
            if (!kruskalLF) {
                kruskalLF = new LeaderFollower(MSTAlgo::Kruskal);
                kruskalLF->start();
            }
            return kruskalLF;
        }
        throw invalid_argument("Algorithm not supported!");
    }

    static void destroyAll() {
        delete primLF;
        delete kruskalLF;
        primLF = nullptr;
        kruskalLF = nullptr;
    }
}; 