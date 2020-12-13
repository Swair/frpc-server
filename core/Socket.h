#ifndef __NBSOCKET_H
#define __NBSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <vector>
#include <mutex>
#include <list>
#include <thread>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <algorithm>
#include <semaphore.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <functional>
#include "Common.h"
#include "Proto.h"
using namespace std::placeholders;

const int MAXCONN  = 5120;

int nbSocket(int domain, int type, int protocol);
void nbBind(int sockfd, const char* ip, const char* port);
void nbListen(int sockfd);
int nbCreateSocket(const char* ip, const char* port);
int nbCreateUdpServer(const char* ip, const char* port);
int nbAccept(int sockfd);
int nbSetnonblocking(int fd);
int nbSafeSend(int fd, char* buf, int len);
int nbSafeRecv(int fd, char* buf, int len);
void nbSendToServer(const char* ip, const char* port, std::function<void(int)>);
void nbConnect(int sockfd, const char* ip, const char* port);



#endif


