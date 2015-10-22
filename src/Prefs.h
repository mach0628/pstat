#ifndef _PREFS_H_
#define _PREFS_H_



class Prefs {
 private:
   char working_dir[MAX_PATH], install_dir[MAX_PATH], startup_dir[MAX_PATH];
   char program_name[64];
  bool daemonize, dump_timeline;
    u_int8_t promiscuousMode;
    u_int ifMTU, snaplen;
	int output_mode;
  char * local_network;
  char *ifName;
  char *data_dir;
  
  char *config_file_path;
  char *packet_filter;
  
  char *redis_host;
  char *pid_path;
  int redis_port;
  
  
  FILE *logFd;

  void SetWorkingDir(char *dir); 
  
  int setOption(int optkey, char *optarg);
  int checkOptions();
  void killDaemon();
  int loadFromFile(const char *path);
  void usage();
public:
	enum{
		MAX_FILTER=16
	};
	u_int32_t filter_ip[MAX_FILTER];
	u_int16_t filter_port[MAX_FILTER];
	int block_mode;
	int host_mode;
	int rid_line;
 public:
  Prefs( char * app);
  ~Prefs();

	inline char* WorkingDir()                     { return(working_dir);      };
	//inline char* InstallDir()                     { return(install_dir);      };
	//inline char* StartupDir()                     { return(startup_dir);      };
	inline char * ProgramName()   { return program_name;};
	char* getValidPath(char *path);

  inline u_int getIfMTU()              { return(ifMTU);             };
  inline u_int8_t getPromiscuousMode() { return(promiscuousMode);   };
  inline u_int getSnaplen()            { return(snaplen);           };
  inline int get_output_mode()    { return output_mode; };
  inline FILE* get_log_fd()                             { return(logFd);                  };
  inline bool do_dump_timeline()                        { return(dump_timeline);          };


  inline char* get_if_name()                    { return ifName; };
  inline char* get_local_network()                    { return local_network; };

  inline char* get_data_dir()                           { return(data_dir);       };

  inline char* get_redis_host()                         { return(redis_host);     }
  inline u_int get_redis_port()                         { return(redis_port);     };


  inline char* get_pid_path()                           { return(pid_path);       };
  inline char* get_packet_filter()                      { return(packet_filter);  };
  
  inline bool isDaemonize()                        { return(daemonize);      };

  int loadFromCLI(int argc,  char *argv[]);

  
  int save();
};
extern Prefs * gPrefs;

#endif /* _PREFS_H_ */
