//
// Created by Barak Rozenkvit on 27/11/2024.
//
#include <mutex>
#include <pthread.h>
#pragma once

class Guard{
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

public:
    Guard();
    ~Guard();
};