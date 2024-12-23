#include "Proactor.hpp"

using namespace std;

pthread_mutex_t pauseMutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for synchronizing access to pause


// Wrapper function.
void* proactorWrapperClient(void* arg) {
    proactorArgsClient* data = static_cast<proactorArgsClient*>(arg);
    void* result = data->func(data->sockfd);
    pthread_mutex_lock(&pauseMutex);
    data-> pause = true;
    pthread_mutex_unlock(&pauseMutex);
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
    pthread_mutex_lock(&pauseMutex);
    data->pause = false;
    pthread_mutex_unlock(&pauseMutex);

    int ret = pthread_create(&thread, nullptr, proactorWrapperClient, data);
    if (ret != 0) {
        perror("pthread_create");
        free(data);
        return make_pair((pthread_t) 0,nullptr);
    }
    return make_pair(thread,data);
}

int stopProactorClient(pthread_t tid,proactorArgsClient* args){
    shutdown(args->sockfd,SHUT_RDWR);
    close(args->sockfd);
    free(args);
    pthread_join(tid, nullptr);
    return 0;
}
