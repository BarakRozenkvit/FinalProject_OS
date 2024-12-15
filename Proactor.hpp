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

typedef void * (* proactorClient) (int sockfd);

typedef void* (* proactorPipeline) (void* stage);

// Struct to hold the original function and its argument
typedef struct {
    proactorClient func;
    int sockfd;
    bool pause;
} proactorArgsClient;

typedef struct {
    proactorPipeline func;
    void* stage;
    bool pause;
} proactorArgsPipeline;

// Wrapper function.
void* proactorWrapperClient(void* arg);

void* proactorWrapperPipeline(void* arg);

// starts new proactor and returns proactor thread id.
pair<pthread_t,void*> startProactorClient(int sockfd, proactorClient threadFunc);

pair<pthread_t,void*> startProactorPipeline(void* stage, proactorPipeline threadFunc);

// stops proactor by threadid
int stopProactorClient(pthread_t tid,proactorArgsClient* args);

int stopProactorPipeline(pthread_t tid,proactorArgsPipeline* args);