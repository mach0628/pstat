
#ifndef _UTILS_H_
#define _UTILS_H_



/* ******************************* */

class Utils {
 private:

 public:
  static char* formatTraffic(float numBits, bool bits, char *buf);
  static char* formatPackets(float numPkts, char *buf);
  static char* l4proto2name(u_int8_t proto);
  
  static char* trim(char *s);
  static u_int32_t hashString(char *s);
  static float timeval2ms(struct timeval *tv);
  static bool mkdir_tree(char *path);
  static void removeTrailingSlash(char *str);
  static void fixPath(char *str);
  
  static void formattime(char * buff,time_t utc);
  static void formattime2(char * buff,time_t utc);
  static char * iptos(u_long in);
  static long time_offset;
};


#endif /* _UTILS_H_ */
