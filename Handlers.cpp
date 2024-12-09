//
// Created by Barak Rozenkvit on 08/12/2024.
//

#include "Handlers.hpp"

std::vector<std::pair<pthread_t,void*>> handlers; // Definition
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;