#include "stdafx.h"
#include "Prefs.h"

#include "Trace.h"
#include "pstat_def.h"
#include "getopt.h"
#include "AdapterFinder.h"
#include "Utils.h"

#ifdef WIN32
#include <shlobj.h> /* SHGetFolderPath() */
#endif

/* ******************************************* */
Prefs * gPrefs=NULL;
#define CONST_DEFAULT_DATA_DIR      "data"

#define DEFAULT_PID_PATH        "/var/tmp/"
#define CONST_DEFAULT_WRITABLE_DIR  "/var/tmp"
#define CONST_DEFAULT_INSTALL_DIR   "/usr/local/share/pstat"
Prefs::Prefs( char * app) {
	char * cp;
	if ((cp = strrchr(app, CONST_PATH_SEP)) != NULL)
		strcpy(program_name ,cp + 1 );
	else strcpy(program_name ,app);
	if ((cp = strrchr(program_name, '.')) != NULL)	cp[0]=0;

#ifdef WIN32

  if(::SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
		     SHGFP_TYPE_CURRENT, working_dir) != S_OK) {
    strcpy(working_dir, "C:\\Windows\\Temp" ); // Fallback: it should never happen
  }

  // Get the full path and filename of this program
  if(GetModuleFileName(NULL, startup_dir, sizeof(startup_dir)) == 0) {
    startup_dir[0] = '\0';	
  } else {
    for(int i=strlen(startup_dir)-1; i>0; i--)
      if(startup_dir[i] == '\\') {
	startup_dir[i] = '\0';
	break;
      }
  }
  strcpy(install_dir, startup_dir);
#else
  struct stat statbuf;

  snprintf(working_dir, sizeof(working_dir), "%s/%s", CONST_DEFAULT_WRITABLE_DIR,program_name);

  umask (0);
  mkdir(working_dir, 0777);

  if(getcwd(startup_dir, sizeof(startup_dir)) == NULL)
    Trace::traceEvent(TRACE_ERROR, "Occurred while checking the current directory (errno=%d)", errno);

  if(stat(CONST_DEFAULT_INSTALL_DIR, &statbuf) == 0)
    strcpy(install_dir, CONST_DEFAULT_INSTALL_DIR);
  else {
    if(getcwd(install_dir, sizeof(install_dir)) == NULL)
      strcpy(install_dir, startup_dir);
  }
#endif

  dump_timeline = false;
  ifMTU = 1514;
  promiscuousMode = 0;
  snaplen = 128;
	output_mode = 1;
  data_dir = strdup(CONST_DEFAULT_DATA_DIR);
  local_network = NULL;
  config_file_path = NULL;

	daemonize = false;

  redis_host = strdup("127.0.0.1");;
  redis_port = 6379;
  logFd = NULL;
  pid_path = new char[strlen(DEFAULT_PID_PATH)+strlen(program_name)+4];
  sprintf(pid_path,"%s%s.pid",DEFAULT_PID_PATH,program_name);
  packet_filter = NULL;

  block_mode =2;
  host_mode=0;
  rid_line=0;
  for(int i=0;i<MAX_FILTER;i++) {
	  filter_ip[i]=0;
	  filter_port[i]=0;
  }
  ifName=NULL;
}

/* ******************************************* */

Prefs::~Prefs() {
  printf("free 1 ");
  if(logFd) fclose(logFd);
  printf("2 ");
  if(data_dir) free(data_dir);
  printf("3 ");
  if( local_network) free(local_network);
  printf("4 ");
  if(config_file_path) free(config_file_path);
  printf("5 ");
  if(pid_path) free(pid_path);
  printf("6 ");
  if( redis_host ) free( redis_host );
  printf("7 \n");
  if( packet_filter ) free( packet_filter);
}


char* Prefs::getValidPath(char *__path) {
  char _path[MAX_PATH];
  struct stat buf;
#ifdef WIN32
  //const char *install_dir = (const char *)install_dir;
#endif
  const char* dirs[] = {
	  startup_dir,
#ifndef WIN32
    CONST_DEFAULT_INSTALL_DIR,
#else
    install_dir,
#endif
    NULL
  };

  if(strncmp(__path, "./", 2) == 0) {
    snprintf(_path, MAX_PATH, "%s/%s", startup_dir, &__path[2]);
	Utils::fixPath(_path);

    if(stat(_path, &buf) == 0)
      return(strdup(_path));
  }

  if((__path[0] == '/') || (__path[0] == '\\')) {
    /* Absolute paths */

    if(stat(__path, &buf) == 0) {
      return(strdup(__path));
    }
  } else
    snprintf(_path, MAX_PATH, "%s", __path);

  /* relative paths */
  for(int i=0; dirs[i] != NULL; i++) {
    char path[MAX_PATH];

    snprintf(path, sizeof(path), "%s/%s", dirs[i], _path);
    Utils::fixPath(path);

    if(stat(path, &buf) == 0) {
      return(strdup(path));
    }
  }

  return(strdup(""));
}

void Prefs::SetWorkingDir(char *dir) 
{
  snprintf(working_dir, sizeof(working_dir), "%s", dir);
  Utils::removeTrailingSlash(working_dir);
}

/* ******************************************* */

/* C-binding needed by Win32 service call */
void Prefs::usage() {
  

  printf("%s %s v.%s (%s) - (C) 2014-06 atdm.liao\n\n"
	 "Usage:\n"
	 "  %s [-f <defaults file>] [-i <local nets>] "
#ifndef WIN32
	 "[-d <data dir>] [-e] "
#endif
#ifdef linux
	 "[-g <core>] "
#endif
	 "[-n mode] [-i <iface|pcap file>]  [-d <path>] \n"
	 "         [-r <redis>] [-v] [-x ] [-C] [-p] [-w]"
#ifdef HAVE_SQLITE
	 " [-F]"
#endif
	 " [-B <filter>] [-A <mode>] [-c <ip>] [-s <length>]\n"
	 "\n"
	 "Options:\n"
	 "[--interface|-i] <interface|pcap>   | Input interface name (numeric/symbolic)\n"
	 "                                    | or pcap file path\n"
#ifndef WIN32
	 "[--data-dir|-d] <path>              | Data directory (must be writable).\n"
	 "                                    | Default: %s\n"
	 "[--daemon|-e]                       | Daemonize pstat\n"
#endif
	 "[--redis|-r] <redis host[:port]>    | Redis host[:port]\n"
	 "[--dump-timeline|-C]                | Enable timeline dump.\n"
#ifndef WIN32
	 "[--pid|-G] <path>                   | Pid file path\n"
#endif
	 "[--kill|-k]                         | Kill Daemon\n"
	 "[--packet-filter|-B] <filter>       | Ingress packet filter (BPF filter)\n"
	 "[--snaplen|-s] <capture length>     | capture length,if length less then 1,set to 128(default:128)\n"
	 "[--output-mode|-A] <mode>           | Setup data output mode:\n"
	 "                                    | bit 0(1) - set 1 to Enable detail file output(default 1)\n"
	 "                                    | bit 1(2) - set 1 to Enable minute sum output(default 0)\n"
	 "                                    | bit 2(4) - set 1 to Enable raw data output(default 0)\n"
#ifdef HAVE_SQLITE
	 "[--dump-flows|-F]                   | Dump on disk expired flows\n"
#endif
	 "[--local-networks|-m] <local nets>  | Local net ip (default: 192.168.1.2)\n"
	 "[--filter-ip|-c] <ip address>       | add ip to be ingored,16 max can be added\n"
	 "[--host-mode|-w]                    | treat filter ip to C class net(default no)\n"
	 "[--filter-port|-p]                  | add port into list to be watch,16 max can be added\\n"
	 "[--block-mode|-n]                   | how to used port and ip list, block mod is 1, pass mode is 0:\n"
	 "                                    | bit 0(1) - port list  mode (default 0 )\n"
	 "                                    | bit 1(2) - ip list  mode (default 1)\n"
	 "[--rid-line|-x] <mode>              | set to 1 to get rid of lines in content(default 0)\n"
	 "[--defaults-file|-f] <file name>    | Use the specified defaults file\n"
	 "[--verbose|-v]                      | Verbose tracing\n"
	 "[--help|-h]                         | Help\n"
	 ,program_name, PACKAGE_MACHINE, PACKAGE_VERSION, NTOPNG_SVN_RELEASE,program_name
#ifndef WIN32
	 ,CONST_DEFAULT_DATA_DIR
#else
	 
#endif
	 );

  printf("\n");
	CAdapterFinder finder;	
	/* Retrieve the device list */
	if(finder.Find()==FALSE)
	{
		exit(1);
	}
	
	/* Print the list */
	finder.PrintAdapterLong(stdout);

  exit(0);
}

/* ******************************************* */

static const struct option long_options[] = {
  { "black-mode",                        required_argument, NULL, 'n' },
  { "interface",                         required_argument, NULL, 'i' },
#ifndef WIN32
  { "data-dir",                          required_argument, NULL, 'd' },
#endif
  { "packet-filter",                     required_argument, NULL, 'B' },
  { "filter-ip",				         required_argument, NULL, 'c' },
  { "dump-timeline",                     no_argument,       NULL, 'C' },
  { "daemonize",                         no_argument,       NULL, 'e' },
  { "host-mode",                         no_argument,       NULL, 'w' },
  { "local-network",                    required_argument,  NULL, 'm' },
  { "filter-port",                      required_argument,  NULL, 'p' },
  { "disable-host-persistency",          no_argument,       NULL, 'P' },
  { "redis",                             required_argument, NULL, 'r' },
  { "core-affinity",                     required_argument, NULL, 'g' },
  { "snaplen",                          required_argument,  NULL, 's' },
  { "disable-login",                     no_argument,       NULL, 'l' },
  { "verbose",                           no_argument,       NULL, 'v' },
  { "help",                              no_argument,       NULL, 'h' },
  { "output-mode",						required_argument,  NULL, 'A' },
  { "defaults-file",					required_argument, NULL,'f' },
#ifdef HAVE_SQLITE
  { "dump-flows",                        no_argument,       NULL, 'F' },
#endif
#ifndef WIN32
  { "pid",                               required_argument, NULL, 'G' },
#endif
  { "kill",                               no_argument, NULL, 'k' },
  { "max-num-flows",                     required_argument, NULL, 'X' },
  { "rid-line",                          required_argument, NULL, 'x' },
  { "user",                              required_argument, NULL, 'U' },
  { "httpdocs-dir",                      required_argument, NULL, '1' },
  { "scripts-dir",                       required_argument, NULL, '2' },
  { "callbacks-dir",                     required_argument, NULL, '3' },
  /* End of options */
  { NULL,                                no_argument,       NULL,  0 }
};

/* ******************************************* */
void Prefs::killDaemon()
{
	FILE *f;
    int pid = 0; /* pid number from pid file */
	fprintf(stderr, "open pid file[%s]\n",pid_path);
    if((f = fopen(pid_path, "r")) == 0){
        fprintf(stderr, "Can't open pid file\n");
		exit(0);
        return ;
    }
    if(fscanf(f, "%d", &pid)!= 1) {
    }
    /* send signal SIGTERM to kill */
    if(pid > 0)  {
#ifndef WIN32
        kill(pid, SIGTERM);
		fprintf(stderr, "process %d killed!\n",pid);
#endif
    }
    fclose(f);
	exit(0);
}
int Prefs::setOption(int optkey, char *optarg) {
  switch(optkey) {
  case 'A':
    switch(atoi(optarg)) {
	  case 2:output_mode =2;break;
	  case 3:output_mode =3;break;
	  case 4:output_mode =4;
		  snaplen = 2048;
		  break;
	  case 5:output_mode =5;
		  snaplen = 2048;
		  break;
	  default:
	  output_mode =1;break;
	}
    break;
  case 'f':
	  if( strlen(optarg)>1 ) loadFromFile(optarg);
	  break;
  case 'B':
	 if( packet_filter ) free(packet_filter);
    packet_filter = strdup(optarg);
    break;

  case 'c':
	  for(int i=0;i< Prefs::MAX_FILTER;i++ ){
		  if( filter_ip[i]==0 ){
			  filter_ip[i]=inet_addr(optarg);
			  break;
		  }
	  }
    break;

  case 'C':
    dump_timeline = true;
    break;

#ifndef WIN32
  case 'd':
    SetWorkingDir(optarg);
    break;
#endif

  case 'e':
    daemonize = true;
    break;

  case 'g':
    
    break;

  case 'm':
    if( local_network ) free(local_network);
    local_network = strdup(optarg);
    break;

  case 'n':
	switch(atoi(optarg)) {
	case 0:
		block_mode=0;
		break;
	case 1:
		block_mode=0;
		break;
	case 2:
		block_mode=0;
		break;
	case 3:
		block_mode=0;
		break;
	default:
		block_mode=2;
		break;
	}
    break;

  case 'p':
	  for(int i=0;i< Prefs::MAX_FILTER;i++ ){
		  if( filter_port[i]==0 ){
			  int k = atoi(optarg);
			  if( k>0 ) filter_port[i]=k;
			  break;
		  }
	  }
    break;

  case 'P':

    break;

  case 'h':
    usage();
    break;

  case 'i':
    if( ifName==NULL)
      ifName = strdup(optarg);
    else
		Trace::traceEvent(TRACE_ERROR, "Too many interfaces: discarded %s", optarg);
    break;

  case 'w':
		host_mode=1;
    break;

  case 'r':
    {
      char buf[64], *r;

      snprintf(buf, sizeof(buf), "%s", optarg);
      r = strtok(buf, ":");
      if(r) {
	char *c = strtok(NULL, ":");
	if(c) redis_port = atoi(c);

	if(redis_host) free(redis_host);
	redis_host = strdup(r);
      }
    }
    break;

  case 's':
    snaplen = atoi(optarg);
	if( snaplen<1 ) snaplen=128;
    break;
  case '1':

    break;

  case '2':

    break;

  case '3':

    break;

  case 'l':
    
    break;

  case 'x':
	  if( (optarg!=NULL )&&(strlen(optarg)>0) ){
		  rid_line=atoi(optarg);
		  if( rid_line<0 ) rid_line=0;
	  } else rid_line = 1;
    break;

  case 'v':
    Trace::set_trace_level(MAX_TRACE_LEVEL);
    break;

#ifdef HAVE_SQLITE
  case 'F':
    dump_flows_on_db = true;
    break;
#endif

#ifndef WIN32
  case 'G':
	if(pid_path) free(pid_path);
    pid_path = strdup(optarg);
    break;
#endif
  case 'k':
	  killDaemon();
	  break;

  case 'U':
    break;

  case 'X':
    break;
      
  default:
    return(-1);
  }

  return(0);
}

/* ******************************************* */

int Prefs::checkOptions() {
  if(daemonize
#ifdef WIN32
		|| 1
#endif
	  ) {
    char path[MAX_PATH];

    //mkdir_s(data_dir, 0777);
    snprintf(path, sizeof(path), "%s/%s.log", install_dir,program_name );
	Utils::fixPath(path);
    logFd = fopen(path, "a");
	if(logFd)
			Trace::traceEvent(TRACE_NORMAL, "Logging into %s", path);
	else
			Trace::traceEvent(TRACE_ERROR, "Unable to create log %s", path);
  }

  
  free(data_dir);data_dir = strdup(install_dir);

  
  if(!data_dir)         { Trace::traceEvent(TRACE_ERROR, "Unable to locate data dir");      return(-1); }
  Trace::traceEvent(TRACE_NORMAL, "locate data dir: %s",data_dir); 
  return(0);
}

/* ******************************************* */

int Prefs::loadFromCLI(int argc, char *argv[]) {
  u_char c;

  while((c = getopt_long(argc, argv, "c:eg:hi:wr:sg:m:n:p:d:x1:2:3:lvA:B:CFG:U:X:kf:",
			 long_options, NULL)) != '?') {
    if(c == 255) break;
    setOption(c, (char *)optarg);
  }

  return(checkOptions());
}

/* ******************************************* */

int Prefs::loadFromFile(const char *path) {
  char buffer[512], *line, *key, *value;
  FILE *fd;
  const struct option *opt;

  config_file_path = strdup(path);

  fd = fopen(config_file_path, "r");

  if(fd == NULL) {
    Trace::traceEvent(TRACE_WARNING, "Config file %s not found", config_file_path);
    return(-1);
  }

  while(fd) {
    if(!(line = fgets(buffer, sizeof(buffer), fd)))
      break;

    line = Utils::trim(line);

    if(strlen(line) < 1 || line[0] == '#')
      continue;

    key = line;
    key = Utils::trim(key);
    
    value = strrchr(line, '=');

    /* Fallback to space */
    if(value == NULL) value = strrchr(line, ' ');
      
    if(value == NULL)
      value = &line[strlen(line)]; /* empty */
    else
      value[0] = 0, value = &value[1];
    value = Utils::trim(value);

    //if(strlen(key) > 2) key = &key[2];
    //else key = &key[1];

    opt = long_options;
    while (opt->name != NULL) {
      if((strcmp(opt->name, key) == 0)
	  || ((key[1] == '\0') && (opt->val == key[0]))) {
        setOption(opt->val, value);
        break;
      }
      opt++;
    }
  }

  fclose(fd);
  return 1;
  //return(checkOptions());
}

/* ******************************************* */

int Prefs::save() {
  FILE *fd;

  if(config_file_path == NULL)
    return(-1);

  fd = fopen(config_file_path, "w");

  if(fd == NULL) {
    Trace::traceEvent(TRACE_ERROR, "Unable to open file %s [%s]", config_file_path, strerror(errno));
    return(-1);
  }

 
  fprintf(fd, "interface=");
  if(ifName)    fprintf(fd, "%s",  ifName);
  fprintf(fd, "\n");

  fprintf(fd,"output-mode=%d\n",output_mode);
  
  if(data_dir)            fprintf(fd, "data-dir=%s\n", data_dir);

  if(local_network)      fprintf(fd, "local-network=%s\n", local_network);  

  if(redis_host)          fprintf(fd, "redis=%s:%d\n", redis_host, redis_port);


  if(Trace::traceLevel != TRACE_LEVEL_NORMAL) fprintf(fd, "verbose\n");

  fclose(fd);

  return(0);
}

/* ******************************************* */

