#include "Proactor.hpp"

using namespace std;

// Wrapper function.
void* proactorWrapper(void* arg) {
    proactorArgs* data = static_cast<proactorArgs*>(arg);
    return data->func(data->sockfd);
}

pair<pthread_t,void*> startProactor(int sockfd, proactorFunc threadFunc) {
    pthread_t thread;
    proactorArgs *data = (proactorArgs *) malloc(sizeof(proactorArgs));
    if (!data) {
        perror("malloc");
        return make_pair((pthread_t) 0,nullptr);
    }

    data->func = threadFunc;
    data->sockfd = sockfd;

    int ret = pthread_create(&thread, nullptr, proactorWrapper, data);
    if (ret != 0) {
        perror("pthread_create");
        free(data);
        return make_pair((pthread_t) 0,nullptr);
    }
    return make_pair(thread,data);
}


int stopProactor(pthread_t tid){
    pthread_kill(tid,0);
    return 0;
}