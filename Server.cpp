//
// Created by Barak Rozenkvit on 04/12/2024.

#include "Algo.hpp"
#include "MainGraph.hpp"
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "Proactor.hpp"
#include "Reactor.hpp"
#include "ClientHandler.hpp"

int get_listener_socket()
{
    int listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listeningSocket == -1)
    {
        perror("socket");
        exit(1);
    }

    int enableReuse = 1;
    int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        perror("setsockopt() failed with error code");
        exit(1);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    int bindResult = ::bind(listeningSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1)
    {
        perror("Bind failed with error code");
        close(listeningSocket);
        exit(1);
    }

    int listenResult = listen(listeningSocket, 10);
    if (listenResult == -1)
    {
        printf("listen() failed with error code");
        close(listeningSocket);
        exit(1);
    }

    return listeningSocket;
}

void signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGKILL)
    {
        std::cout << "shutting down gracefully..." << std::endl;
        pthread_mutex_lock(&mtx);
        for(auto id:handlers){
            pthread_kill(id.first,0);
            proactorArgs* data = static_cast<proactorArgs*>(id.second);
            close(data->sockfd);
            free(data);
        }
        pthread_mutex_unlock(&mtx);
        exit(0);
    }
}

int main()
{

    MainGraph *graph = MainGraph::getInstance();

    signal(SIGINT, signalHandler);
    signal(SIGKILL, signalHandler);

    ClientHandler::monitorHandlers();

    int listener = get_listener_socket();
    if (listener == -1)
    {
        perror("socket");
        exit(1);
    }

    Reactor *reactor = startReactor();
    addFdToReactor(reactor, listener, ClientHandler::handleConnection);
    while (reactor->run)
    {
        int poll_count = poll(reactor->pfds, reactor->fd_count, -1);
        if (poll_count == -1){
            if (errno == EINTR){
                signalHandler(SIGINT);
            }
            perror("poll");
        }

        for (int i = 0; i < reactor->fd_count; i++)
        {
            if (reactor->pfds[i].revents & POLLIN && reactor->pfds[i].fd != -1)
            {
                int hot_fd = reactor->pfds[i].fd;
                if (listener == hot_fd)
                {
                    void *fds = reactor->f2f[i].func(hot_fd);
                    int res = *(int *)fds;
                    free(fds);
                    if (res == -1)
                    {
                        continue;
                    }
                }
            }
        }
    }
}