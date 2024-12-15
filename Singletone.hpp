#include <pthread.h>
#pragma once

template <typename T>
class Singletone{

    static T* instance;
    static pthread_mutex_t mutex;

    Singletone();
    ~Singletone();

public:

    static T* getInstance(){
        pthread_mutex_lock(&Singletone::mutex);
        if (Singletone::instance == nullptr){
            Singletone::instance = new T();
        }
        pthread_mutex_unlock(&Singletone::mutex);
        return Singletone::instance;
    }

    static void destroyInstance(){
        pthread_mutex_lock(&Singletone::mutex);
        if (Singletone::instance){
            delete Singletone::instance;
            Singletone::instance = nullptr;
        }
        pthread_mutex_unlock(&Singletone::mutex); 
    }

    static void lockInstance(){
        pthread_mutex_lock(&Singletone::mutex);
    }

    static void unlockInstance(){
        pthread_mutex_unlock(&Singletone::mutex);
    }
};

template <typename T>
T* Singletone<T>::instance = nullptr;

template <typename T>
pthread_mutex_t Singletone<T>::mutex = PTHREAD_MUTEX_INITIALIZER;