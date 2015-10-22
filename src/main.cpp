// nstat.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "PortStat.h"
#include "Trace.h"
#include "Utils.h"

/* ******************************** */

void sigproc(int sig) {
  static int called = 0;
	Trace::traceEvent(TRACE_NORMAL, "Receive signal %d.",sig);
  if(called) {
	  Trace::traceEvent(TRACE_NORMAL, "Ok I am leaving now");
    exit(0);
  } else {
    Trace::traceEvent(TRACE_NORMAL, "Shutting down...");
    called = 1;
  }

  pstat->Close();
  //sleep(1); /* Wait until all threads know that we're shutting down... */

#ifndef WIN32
  if(gPrefs->isDaemonize()){
	  if(gPrefs->get_pid_path() != NULL) {
		int rc = unlink(gPrefs->get_pid_path());
		Trace::traceEvent(TRACE_NORMAL, "Deleted PID %s [rc: %d]", 
					 gPrefs->get_pid_path(), rc);
	  }
  }
#endif
  //if( pstat ) delete pstat;
  Trace::traceEvent(TRACE_NORMAL, "Ok signal finish now");
 // exit(0);
}

/* ******************************************* */

int main(int argc, char *argv[])
{
  
  tzset();

  if((gPrefs = new Prefs(argv[0])) == NULL)  exit(0);

  if((pstat = new PortStat()) == NULL) exit(0);

  if( gPrefs->loadFromCLI(argc, argv) < 0) return(-1);

  pstat->SetPrefs(gPrefs);

  if( !pstat->Create() ) return -1;

  if(gPrefs->isDaemonize()) {  
	  pstat->Daemonize();
#ifndef WIN32
  if(gPrefs->get_pid_path() != NULL) {
    FILE *fd = fopen(gPrefs->get_pid_path(), "w");
    if(fd != NULL) {
      fprintf(fd, "%u\n", getpid());
      fclose(fd);
      chmod(gPrefs->get_pid_path(), 0777);
      Trace::traceEvent(TRACE_NORMAL, "PID stored in file %s", gPrefs->get_pid_path());
    } else
      Trace::traceEvent(TRACE_ERROR, "Unable to store PID in file %s", gPrefs->get_pid_path());
  }
#endif
  }

  /*
    We have created the network interface and thus changed user. Let's not check
    if we can write on the data directory
  */
  {
    char path[MAX_PATH];
    FILE *fd;

    snprintf(path, sizeof(path), "%s/.test", gPrefs->WorkingDir());
	Utils::fixPath(path);

    if((fd = fopen(path, "w")) == NULL) {
      Trace::traceEvent(TRACE_ERROR,
				   "Unable to write on %s [%s]: please specify a different directory (-d)",
				   gPrefs->WorkingDir(), path);
      exit(0);
    } else {
      fclose(fd); /* All right */
      unlink(path);
    }
  }

  Trace::traceEvent(TRACE_NORMAL, "Working directory: %s", gPrefs->WorkingDir());


  signal(SIGINT, sigproc);
  signal(SIGTERM, sigproc);
  signal(SIGINT, sigproc);

  pstat->Run();
  Trace::traceEvent(TRACE_NORMAL, "main loop closed!");
  pstat->Close();
  delete pstat;
  delete gPrefs;
  gPrefs = 0;
  Trace::traceEvent(TRACE_NORMAL, "main finish");
  return(0);
}

