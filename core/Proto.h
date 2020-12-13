#ifndef __NBPROTO_H
#define __NBPROTO_H
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <vector>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <tuple>
#include "Common.h"




int sendRequest(int fd, Fson& fson);
int recvRequest(int fd, Fson& fson);





#endif


