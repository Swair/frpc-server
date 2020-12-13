#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "LogWrite.h"


void log_write(const char* file, const int line, const char* func, const char* fmt, ...)
{
    va_list vp;
    pid_t pid = getpid();
    pthread_t tid = pthread_self();

    char timeStamp[64] = {0};
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    strftime(timeStamp, sizeof(timeStamp)-1, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));

    char logBuf[10240] = {0};
    size_t res1 = sprintf(logBuf, "%s [%d,%u, %15s:%d, %15s]: ", timeStamp, pid, (unsigned)tid, file, line, func); 

    va_start(vp, fmt);
    size_t res2 = vsnprintf(logBuf + res1, sizeof(logBuf) - res1 - 1, fmt, vp); 
    va_end(vp);

    if((res1 + res2 + 1) >= sizeof(logBuf))
        printf("the log size is too larg\n");

    printf("%s",logBuf);
}



