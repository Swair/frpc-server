#ifndef __NBREACTOR_H
#define __NBREACTOR_H

#include "Socket.h"
#include "Common.h"
#include "ThreadPool.h"

const int SOCKET_CLOSE = 1;
const int SOCKET_KEEPALIVE = 2;
const int SOCKET_EXCEPTION = 4;

const int MAXEVENT = 5120;

typedef struct
{
    int efd;   
    int fd;
    int sfd;
    int cfd;
    std::function<void(void*)> cb;
} UEvent;


class Reactor
{
    public:
        Reactor(int threadNums);
        ~Reactor();
        void epollCtrl(int efd, int ctrl, int fd, int event, const UEvent* uevt);
        void initServer(const char* ip, const char* port);
        void runEpollServer(const char* ip, const char* port, std::function<int(int)> userCallBack);
        void accepthandler(const void* args);
        void processhandler(void* args);
        void closehandler(void* args);
        void exphandler(void* args);
        std::vector<char> work(const UEvent* uevt, const std::vector<char>& requestBuf);
        int getBody(int cfd, Fson& request);

    private: 
        int efd_;
        bool workRun_;
        std::function<int(int)> userCallBack_;
        int recvBufSize_;
        std::unique_ptr<ThreadPool> threadPool_;
};

#endif


