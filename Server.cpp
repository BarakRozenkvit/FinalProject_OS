#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>
#include <filesystem>
#include <string>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <pthread.h>

#include <mach/mach.h>
#include "Reactor.hpp"
#include "ClientHandler.hpp"
#include "Graph.hpp"
namespace fs = std::filesystem;

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

void listThreads() {
    // Get the current task
    mach_port_t task;
    task_for_pid(mach_task_self(), getpid(), &task);

    // Get thread list
    thread_act_array_t threadList;
    mach_msg_type_number_t threadCount;

    kern_return_t kr = task_threads(task, &threadList, &threadCount);
    if (kr != KERN_SUCCESS) {
        std::cerr << "Error: Unable to get thread list. Error code: " << kr << std::endl;
        return;
    }

    std::cout << "Total threads: " << threadCount << std::endl;
    for (mach_msg_type_number_t i = 0; i < threadCount; ++i) {
        std::cout << "Thread ID: " << threadList[i] << std::endl;
    }

    // Deallocate thread list
    vm_deallocate(mach_task_self(), reinterpret_cast<vm_address_t>(threadList), threadCount * sizeof(thread_act_t));
}

void signalHandler(int signal){
    if (signal == SIGINT || signal == SIGKILL){
        std::cout << "shutting down gracefully..." << std::endl;
        ClientHandler::killHandlers();
        Pipeline::killWorkers();

        pid_t pid = getpid(); // Get the current process ID
        std::cout << "Process ID: " << pid << std::endl;

        std::cout << "Threads in this process:" << std::endl;
        listThreads();        
        exit(0);
    }
}

int main()
{


    signal(SIGINT, signalHandler);
    signal(SIGKILL, signalHandler);

    ClientHandler::startMonitorHandlers();

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