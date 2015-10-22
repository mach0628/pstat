

#ifndef _TRACE_H_
#define _TRACE_H_



#define TRACE_LEVEL_ERROR     0
#define TRACE_LEVEL_WARNING   1
#define TRACE_LEVEL_NORMAL    2
#define TRACE_LEVEL_INFO      3
#define TRACE_LEVEL_DEBUG     6

#define TRACE_ERROR     TRACE_LEVEL_ERROR, __FILE__, __LINE__
#define TRACE_WARNING   TRACE_LEVEL_WARNING, __FILE__, __LINE__
#define TRACE_NORMAL    TRACE_LEVEL_NORMAL, __FILE__, __LINE__
#define TRACE_INFO      TRACE_LEVEL_INFO, __FILE__, __LINE__
#define TRACE_DEBUG     TRACE_LEVEL_DEBUG, __FILE__, __LINE__

#define MAX_TRACE_LEVEL 6
#define TRACE_DEBUGGING MAX_TRACE_LEVEL

class Mutex; /* Forward */

/* ******************************* */

class Trace {
 public:
  static u_int8_t traceLevel;

 public:

  static  void set_trace_level(u_int8_t id);
  static void traceEvent(int eventTraceLevel, const char* file, const int line, const char * format, ...);
};


#endif /* _TRACE_H_ */
