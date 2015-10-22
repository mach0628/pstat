
#include "stdafx.h"
#include "Utils.h"
#include "pstat_def.h"



long Utils::time_offset =  timezone ;//- (daylight * 3600);
/* ****************************************************** */

char* Utils::formatTraffic(float numBits, bool bits, char *buf) {
  char unit;

  if(bits)
    unit = 'b';
  else
    unit = 'B';

  if(numBits < 1024) {
    snprintf(buf, 32, "%lu %c", (unsigned long)numBits, unit);
  } else if(numBits < 1048576) {
    snprintf(buf, 32, "%.2f K%c", (float)(numBits)/1024, unit);
  } else {
    float tmpMBits = ((float)numBits)/1048576;

    if(tmpMBits < 1024) {
      snprintf(buf, 32, "%.2f M%c", tmpMBits, unit);
    } else {
      tmpMBits /= 1024;

      if(tmpMBits < 1024) {
	snprintf(buf, 32, "%.2f G%c", tmpMBits, unit);
      } else {
	snprintf(buf, 32, "%.2f T%c", (float)(tmpMBits)/1024, unit);
      }
    }
  }

  return(buf);
}

/* ****************************************************** */

char* Utils::formatPackets(float numPkts, char *buf) {
  if(numPkts < 1000) {
    snprintf(buf, 32, "%.2f", numPkts);
  } else if(numPkts < 1000000) {
    snprintf(buf, 32, "%.2f K", numPkts/(float)1000);
  } else {
    numPkts /= 1000000;
    snprintf(buf, 32, "%.2f M", numPkts);
  }

  return(buf);
}

/* ****************************************************** */

char* Utils::l4proto2name(u_int8_t proto) {
  static char proto_string[8];

  switch(proto) {
  case 0:   return((char*)"IP");
  case 1:   return((char*)"ICMP");
  case 2:   return((char*)"IGMP");
  case 6:   return((char*)"TCP");
  case 17:  return((char*)"UDP");
  case 47:  return((char*)"GRE");
  case 112: return((char*)"VRRP");

  default:
    snprintf(proto_string, sizeof(proto_string), "%u", proto);
    return(proto_string);
  }
}

#define IPTOSBUFFERS	24
	static char output[IPTOSBUFFERS][3*4+3+1];
	static short which;
char * Utils::iptos(u_long in)
{

	u_char *p;

	p = (u_char *)&in;
	which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
	sprintf(output[which], "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
	return output[which];
}
/* ****************************************************** */

char *Utils::trim(char *s) {
  char *end;

  while(isspace(s[0])) s++;
  if(s[0] == 0) return s;

  end = &s[strlen(s) - 1];
  while(end > s && isspace(end[0])) end--;
  end[1] = 0;

  return s;
}

/* ****************************************************** */

u_int32_t Utils::hashString(char *key) {
  u_int32_t hash = 0, len = strlen(key);

  for(u_int32_t i=0; i<len; i++)
    hash += ((u_int32_t)key[i])*i;

  return(hash);
}

/* ****************************************************** */

float Utils::timeval2ms(struct timeval *tv) {
  return((float)tv->tv_sec*1000+(float)tv->tv_usec/1000);
}

/* ****************************************************** */

bool Utils::mkdir_tree(char *path) {
  int permission = 0777, i, rc;
  struct stat s;

  fixPath(path);

  if(stat(path, &s) != 0) {
    /* Start at 1 to skip the root */
    for(i=1; path[i] != '\0'; i++)
      if(path[i] == CONST_PATH_SEP) {
#ifdef WIN32
	/* Do not create devices directory */
	if((i > 1) && (path[i-1] == ':')) continue;
#endif

	path[i] = '\0';
	rc = mkdir_s(path, permission);
	path[i] = CONST_PATH_SEP;
      }

    rc = mkdir_s(path, permission);

    return(rc == 0 ? true : false);
  } else
    return(true); /* Already existing */
}

void Utils::removeTrailingSlash(char *str) {
  int len = strlen(str)-1;

  if((len > 0)
     && ((str[len] == '/') || (str[len] == '\\')))
    str[len] = '\0';
}

void Utils::formattime(char * buff,time_t utc)
{
	struct tm l;
	localtime_r(&utc,&l);
	sprintf(buff,"%d-%02d-%02d %02d:%02d:%02d",l.tm_year+1900,l.tm_mon+1,l.tm_mday,l.tm_hour,l.tm_min,l.tm_sec);
}
static   char *log_month[12]={ "Jan", "Feb", "Mar",
                         "Apr", "May", "Jun",
                         "Jul", "Aug", "Sep",
                         "Oct", "Nov", "Dec"};
void Utils::formattime2(char * buff,time_t utc)
{
	struct tm l;
	localtime_r(&utc,&l);
	//23/Jun/2014:09:41:07 +0800
	sprintf(buff,"%02d/%s/%d:%d:%02d:%02d +0800",l.tm_mday,log_month[l.tm_mon],l.tm_year+1900,l.tm_hour,l.tm_min,l.tm_sec); 
}
/* ******************************************* */

void Utils::fixPath(char *str) {
#ifdef WIN32
  for(int i=0; str[i] != '\0'; i++)
    if(str[i] == '/') str[i] = '\\';
#endif
}

