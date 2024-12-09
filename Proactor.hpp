#include <map>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <vector>
#include <utility>

#pragma once

using namespace std;

typedef void * (* proactorFunc) (int sockfd);

// Struct to hold the original function and its argument
typedef struct {
    proactorFunc func;
    int sockfd;
    bool pause;
} proactorArgs;

// Wrapper function.
void* proactorWrapper(void* arg) ;

// starts new proactor and returns proactor thread id.
pair<pthread_t,void*> startProactor(int sockfd, proactorFunc threadFunc);

// stops proactor by threadid
int stopProactor(pthread_t tid,proactorArgs* args) ;