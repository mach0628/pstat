// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						


// TODO: 在此处引用程序需要的其他头文件
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>

#ifdef WIN32
#include "nstat_win32.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>

#ifdef USE_SYSLOG
#include <syslog.h>
#endif

#include <netdb.h>
#include <dirent.h>
#include <pwd.h>
typedef int                 BOOL;
typedef void *HANDLE;

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#endif

#ifdef linux
#define __FAVOR_BSD
#endif


#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#ifndef MAX_PATH
#define MAX_PATH                256
#endif
#define HAVE_PTHREAD_H


