
#ifndef _NSTAT_DEFINES_H_
#define _NSTAT_DEFINES_H_



#ifdef WIN32
#define mkdir_s(a, b) _mkdir(a)
#define CONST_PATH_SEP                    '\\'
#else
#define mkdir_s(a, b) mkdir(a, b)
#define CONST_PATH_SEP                    '/'
#endif




#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#ifdef WIN32

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#define MAX_NUM_QUEUED_ADDRS    500 /* Maximum number of queued address for resolution */

  // =========================================================
  // You should not need any changes below this line
  // =========================================================

  // Value name for app parameters
#define SZAPPPARAMS "AppParameters"

  // list of service dependencies - "dep1\0dep2\0\0"
  // If none, use ""
#define SZDEPENDENCIES ""
#endif

/* SVN Release */
#define NTOPNG_SVN_RELEASE "r6932"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Architecture of this host */
#define PACKAGE_MACHINE "x86_64"
/* SVN release of this package */
#define PACKAGE_RELEASE "r6932"
/* Define to the version of this package. */
#define PACKAGE_VERSION "1.2"
#ifndef min_val
#define min_val(a,b) ((a < b) ? a : b)
#endif

#ifndef max_val
#define max_val(a,b) ((a > b) ? a : b)
#endif

#endif /* _NTOP_DEFINES_H_ */
