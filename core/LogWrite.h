#ifndef __LOGWRITE_H
#define __LOGWRITE_H

#include <stdio.h>

#define logWrite(fmt,...) (log_write(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__))
void log_write(const char* file, const int line, const char* func, const char* fmt, ...);

#endif

