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

// Function type for clients threads
typedef void * (* proactorClient) (int sockfd);

// Function type for active objects threads
typedef void* (* proactorPipeline) (void* stage);

// Structs to hold the original function and its argument
typedef struct {
    proactorClient func;
    int sockfd;
    bool pause;
} proactorArgsClient;

typedef struct {
    proactorPipeline func;
    void* stage;
} proactorArgsPipeline;

// Wrapper functions for proactor
void* proactorWrapperClient(void* arg);

void* proactorWrapperPipeline(void* arg);

// starts new proactor and returns proactor thread id and ThreadArgs.
pair<pthread_t,void*> startProactorClient(int sockfd, proactorClient threadFunc);

pair<pthread_t,void*> startProactorPipeline(void* stage, proactorPipeline threadFunc);

// stops proactor by threadid and free Thread Args
int stopProactorClient(pthread_t tid,proactorArgsClient* args);

int stopProactorPipeline(pthread_t tid,proactorArgsPipeline* args);