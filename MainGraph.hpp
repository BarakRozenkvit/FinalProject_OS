#include <mutex>
#include <pthread.h>
#include "Graph.hpp"


class MainGraph: public Graph{
private:
    static MainGraph* instance;
    static pthread_mutex_t mutex;

    MainGraph();
    ~MainGraph();

public:

    static MainGraph* getInstance();

    static void destroyInstance();

    static void lockInstance();

    static void unlockInstance();

};
