#ifndef _NTOP_WIN32_H_
#define _NTOP_WIN32_H_

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
 
#if _MSC_VER 
#define snprintf _snprintf 
#endif
#include <time.h>
#include <winsock2.h> /* winsock.h is included automatically */
#include <ws2tcpip.h>
#include <process.h>
#include <io.h>
#include <stdio.h>


#include <process.h> /* for getpid() and the exec..() family */
#include <direct.h>  /* mkdir */

/* Values for the second argument to access. These may be OR'd together.  */
#define R_OK    4       /* Test for read permission.  */
#define W_OK    2       /* Test for write permission.  */
//#define   X_OK    1       /* execute permission - unsupported in windows*/
#define F_OK    0       /* Test for existence.  */

#define access _access
#define ftruncate _chsize

/* Damn XP */
#define strdup(a) _strdup(a)


/* getopt.h */
#define __GNU_LIBRARY__ 1

#ifndef __GNUC__
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   uint;
typedef unsigned long  u_long;
#endif

typedef u_char  u_int8_t;
typedef u_short u_int16_t;
typedef uint   u_int32_t;
typedef int   int32_t;
typedef unsigned __int64 u_int64_t;
typedef __int64 int64_t;


#define localtime_r(a, b) localtime_s(b,a)

#define sleep(a) Sleep(a*1000)
//extern unsigned int sleep(unsigned int seconds);

//extern int inet_aton(const char *cp, struct in_addr *addr);
//extern int inet_pton(int af, const char *src, void *dst);


#endif /* _NTOP_WIN32_H_ */

