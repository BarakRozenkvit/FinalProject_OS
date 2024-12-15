#include <pthread.h>
#pragma once

template <typename T>
class Singletone{

    // private instance of T
    static T* instance;
    // mutex of singletone
    static pthread_mutex_t mutex;

    Singletone();
    ~Singletone();

public:

    // get the instance of the class
    static T* getInstance(){
        pthread_mutex_lock(&Singletone::mutex);
        if (Singletone::instance == nullptr){
            Singletone::instance = new T();
        }
        pthread_mutex_unlock(&Singletone::mutex);
        return Singletone::instance;
    }

    // destroy the instance of the class
    static void destroyInstance(){
        pthread_mutex_lock(&Singletone::mutex);
        if (Singletone::instance){
            delete Singletone::instance;
            Singletone::instance = nullptr;
        }
        pthread_mutex_unlock(&Singletone::mutex); 
    }

    // lock instance
    static void lockInstance(){
        pthread_mutex_lock(&Singletone::mutex);
    }

    // unlock instance
    static void unlockInstance(){
        pthread_mutex_unlock(&Singletone::mutex);
    }
};

// static assignments
template <typename T>
T* Singletone<T>::instance = nullptr;

template <typename T>
pthread_mutex_t Singletone<T>::mutex = PTHREAD_MUTEX_INITIALIZER;