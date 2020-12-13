#ifndef __NBCOMMON_H
#define __NBCOMMON_H

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
#include "LogWrite.h"
#include "String.h"
#include "Fson.h"
#define assert(x, str) \
    do{\
        if(x < 0)\
        {   perror(str); exit(-1);}\
    }while(0)


#define DEBUG 0



#endif

