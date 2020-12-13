#include "Socket.h"

int nbSocket(int domain, int type, int protocol)
{
    int sfd = ::socket(domain, type, protocol);
    assert(sfd, strerror(errno));
    return sfd;
}

void nbBind(int sfd, const char* ip, const char* port)
{
    int iport = atoi(port); 
    int reuse = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    //s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_addr.s_addr = inet_addr(ip);
    s_addr.sin_port = htons(iport);
    int res = ::bind(sfd, (struct sockaddr*)(&s_addr), sizeof(struct sockaddr));
    assert(res, strerror(errno));

    if(iport == 0)
    {
        socklen_t slen = sizeof(s_addr);
        res = getsockname(sfd, (struct sockaddr* )&s_addr, &slen);
        assert(res, strerror(errno));
        iport = (int)ntohs(s_addr.sin_port);
    }

    logWrite("bind server ip(%s), port(%d)\n", inet_ntoa(s_addr.sin_addr), iport);
}

void nbListen(int sfd)
{
    int res = ::listen(sfd, MAXCONN);
    assert(res, strerror(errno));
}

int nbAccept(int sfd)
{
    int cfd = -1;
    struct sockaddr_in c_addr;
    socklen_t sin_size = sizeof(c_addr);
    cfd = ::accept(sfd, (struct sockaddr*)&c_addr, &sin_size);
    if(cfd > 0)
        logWrite("Server get connection from %s:%d\n",inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
    return cfd;
}

int nbCreateSocket(const char* ip, const char* port)
{
    int sfd = nbSocket(AF_INET, SOCK_STREAM, 0);
    //int sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    nbBind(sfd, ip, port);
    nbListen(sfd);
    return sfd;
}

int nbCreateUdpServer(const char* ip, const char* port)
{
    int udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    nbBind(udpfd, ip, port); 
    return udpfd;
}

void nbConnect(int sfd, const char* ip, const char* port)
{
    int res = -1; 
    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(ip);
    s_addr.sin_port = htons(atoi(port));
    res = ::connect(sfd, (struct sockaddr*)&s_addr, sizeof(struct sockaddr));
    assert(res, strerror(errno)); 
    logWrite("Connect server %s:%s\n", ip, port);
}

int nbSetnonblocking(int fd)
{
    int oldOption = fcntl(fd, F_GETFL);
    int newOption = oldOption | O_NONBLOCK;
    fcntl(fd, F_SETFL, newOption);
    return oldOption;
}

int nbSafeSend(int fd, char* buf, int len)
{
    if(buf == NULL || len <= 0)
    {
        logWrite("nbSafeSend, no data in buf\n");
        close(fd);
        return -1;
    }

    char* wbuf = buf;
    int wlen = len;
    int accWriteNum = 0; 
    int offset = 0;
    while(true)
    {
        int res = send(fd, wbuf + offset, wlen - offset, MSG_NOSIGNAL);
        if(res > 0)
        {
#if DEBUG
            logWrite("-------------res(%d) >> ", res);
            for(int i = 0; i < wlen; i++)
                logWrite("[0x%x]", wbuf[i]);
            logWrite("\n");
#endif
            offset = res;
            accWriteNum += offset;
            //logWrite("accWriteNum %d\n", accWriteNum);
            if(accWriteNum < wlen)
                continue;
            else
                break;
        }
        else if(res < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            {
                logWrite("----------read empty and end, res = %d\n", res);
            }
            else
            {
                logWrite("---------no data recv, res = %d\n", res);
                return -1;
            }
            continue;
        }
        else
            continue;
    }
    return accWriteNum;
}

int nbSafeRecv(int fd, char* buf, int len)
{
    if(buf == NULL || len <= 0)
    {
        logWrite("nbSafeRecv, no data in buf\n");
        close(fd);
        return -1;
    }

    char* rbuf = buf;
    int rlen = len;
    int accReadNum = 0;
    int offset = 0;

    int readCount = 0;
    while(true)
    {
        int res = recv(fd, rbuf + offset, rlen - offset, 0);
#if DEBUG 
        logWrite("res = %d, offset = %d, rlen = %d\n", res, offset, rlen);
#endif
        if(res > 0)
        {
#if DEBUG
            logWrite("-------------res(%d) << ", res);
            for(int i = 0; i < rlen; i++)
                logWrite("[0x%x]", rbuf[i]);
            logWrite("\n");
#endif

            offset = res;
            accReadNum += offset;
            if(accReadNum < rlen)
                continue;
            else
                break;
        }
        else if(res == 0)
        {
            close(fd);
#if DEBUG
            logWrite("----------client is closed\n");
#endif
            return 0;
        }
        else
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            {
                //logWrite("----------read empty and end, res = %d\n", res);
                if(readCount++ > 500)
                {
                    close(fd);
#if DEBUG
                    logWrite("----------client is closed\n");
#endif
                    return -1;
                }
#if DEBUG
                logWrite("---------continue\n");
#endif
                usleep(10*1000);
                continue; 
            }
            else
            {
                logWrite("---------no data recv, res = %d\n", res);
                close(fd);
#if DEBUG
                logWrite("----------client is closed\n");
#endif
                return -2;
            }
        }
    }
    return accReadNum;
}

void nbSendToServer(const char* ip, const char* port, std::function<void(int)> userCallBack)
{
    int sfd = nbSocket(AF_INET, SOCK_STREAM, 0);
    nbConnect(sfd, ip, port);
    userCallBack(sfd);
    close(sfd);
}


