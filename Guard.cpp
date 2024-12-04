#include "Guard.hpp"

Guard::Guard() {
    pthread_mutex_lock(&mutex);
}

Guard::~Guard() {
    pthread_mutex_unlock(&mutex);
}
