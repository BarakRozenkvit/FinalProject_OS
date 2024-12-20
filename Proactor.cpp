#include "Proactor.hpp"

using namespace std;

// Wrapper function.
void* proactorWrapperClient(void* arg) {
    proactorArgsClient* data = static_cast<proactorArgsClient*>(arg);
    void* result = data->func(data->sockfd);
    data-> pause = true;
    return result;
}

pair<pthread_t,void*> startProactorClient(int sockfd, proactorClient threadFunc) {
    pthread_t thread;
    proactorArgsClient *data = (proactorArgsClient *) malloc(sizeof(proactorArgsClient));
    if (!data) {
        perror("malloc");
        return make_pair((pthread_t) 0,nullptr);
    }

    data->func = threadFunc;
    data->sockfd = sockfd;
    data->pause = false;
    int ret = pthread_create(&thread, nullptr, proactorWrapperClient, data);
    if (ret != 0) {
        perror("pthread_create");
        free(data);
        return make_pair((pthread_t) 0,nullptr);
    }
    return make_pair(thread,data);
}

int stopProactorClient(pthread_t tid,proactorArgsClient* args){
    pthread_kill(tid,0);
    free(args);
    return 0;
}
