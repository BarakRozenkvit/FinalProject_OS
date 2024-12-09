//
// Created by Barak Rozenkvit on 08/12/2024.
//

#ifndef FINALPROJECT_OS_HANDLERS_HPP
#define FINALPROJECT_OS_HANDLERS_HPP

#pragma once
#include <vector>
#include <pthread.h>
#include <utility>

extern std::vector<std::pair<pthread_t,void*>> handlers; // Declaration
extern pthread_cond_t cond;
extern pthread_mutex_t mtx;

#endif //FINALPROJECT_OS_HANDLERS_HPP
