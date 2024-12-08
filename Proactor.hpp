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
#include "MainGraph.hpp"

#pragma once

using namespace std;

typedef void * (* proactorFunc) (int sockfd);

// Struct to hold the original function and its argument
typedef struct {
    proactorFunc func;
    int sockfd;
    MainGraph* graph;
} ThreadData;

// Wrapper function.
void* thread_wrapper(void* arg) ;

// starts new proactor and returns proactor thread id.
pthread_t startProactor(int sockfd, proactorFunc threadFunc);

// stops proactor by threadid
int stopProactor(pthread_t tid) ;