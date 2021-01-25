#include "Reactor.h"


Reactor::Reactor(int threadNums):
    workRun_(true)
{
    threadPool_ = std::make_unique<ThreadPool>(threadNums);
}

Reactor::~Reactor()
{
    workRun_ = false;
}

void Reactor::epollCtrl(int efd, int ctrl, int fd, int event, const UEvent* uevt)
{
    struct epoll_event evt;
    evt.events = event;
    evt.data.ptr = (void*)uevt;
    epoll_ctl(efd, ctrl, fd, &evt); 
}

void Reactor::processhandler(void* args)
{
    UEvent* cevt = (UEvent*)args;
    int efd = cevt->efd;
    int cfd = cevt->cfd;
    int state = userCallBack_(cfd);
    switch(state)
    {
        case SOCKET_CLOSE: 
            close(cfd);
        case SOCKET_EXCEPTION: 
            delete cevt; 
            epollCtrl(efd, EPOLL_CTL_DEL, cfd, 0, cevt);
            break;
        case SOCKET_KEEPALIVE:
        default:
            epollCtrl(efd, EPOLL_CTL_MOD, cfd, EPOLLIN | EPOLLET | EPOLLONESHOT, cevt);
    }
}

void Reactor::exphandler(void* args)
{
    UEvent* cevt = (UEvent*)args;
    int efd = cevt->efd;
    int cfd = cevt->cfd;
    logWrite("exception\n");
    close(cfd);
    delete cevt; 
    epollCtrl(efd, EPOLL_CTL_DEL, cfd, 0, cevt);
}

void Reactor::initServer(const char* ip, const char* port)
{
    efd_ = epoll_create(MAXEVENT);
    assert(efd_, "epoll_create"); 
    int sfd = nbCreateSocket(ip, port);
    assert(sfd, "createSocket");

    UEvent* sevt = new UEvent();
    sevt->efd = efd_; 
    sevt->fd = sfd; 
    sevt->sfd = sfd; 
    sevt->cb = std::move(std::bind(&Reactor::accepthandler, this, _1)); 
    sevt->cfd = 0; 
    epollCtrl(efd_, EPOLL_CTL_ADD, sfd, EPOLLIN | EPOLLET, sevt);
    nbSetnonblocking(sfd); 
}

void Reactor::accepthandler(const void* args)
{
    UEvent* sevt = (UEvent*)args; 
    int efd = sevt->efd;
    int sfd = sevt->sfd;

    while(true)
    {
        int cfd = nbAccept(sfd);
        if(cfd <= 0)
            return;
        UEvent* cevt = new UEvent();
        cevt->efd = efd; 
        cevt->fd = cfd;
        cevt->sfd = 0; 
        cevt->cfd = cfd;
        cevt->cb = std::move(std::bind(&Reactor::processhandler, this, _1));
        epollCtrl(efd, EPOLL_CTL_ADD, cfd, EPOLLIN | EPOLLET | EPOLLONESHOT, cevt);
        nbSetnonblocking(cfd); 
    }
}

void Reactor::runEpollServer(const char* ip, const char* port, std::function<int(int)> userCallBack)
{
    userCallBack_ = userCallBack;
    struct epoll_event events[MAXEVENT];
    initServer(ip, port); 
    while(workRun_)
    {
        int activedLinkNum = epoll_wait(efd_, events, MAXEVENT, -1);
        assert(activedLinkNum, "error in epoll_wait"); 
        for(int i = 0; i < activedLinkNum; i++)
        {
            void* uevt = (void*)events[i].data.ptr;
            threadPool_->enqueue([uevt]{(((UEvent*)uevt)->cb)(uevt);});
        }
    }
}







