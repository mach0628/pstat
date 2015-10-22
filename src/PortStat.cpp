#include "stdafx.h"
#include "PortStat.h"
#include "Utils.h"
#include "PcapInput.h"
#include "Trace.h"
#include "Output.h"
#include "AdapterFinder.h"
#include "pstat_def.h"



PortStat * pstat=NULL;

PortStat::PortStat()
{
   mInputer = NULL;
   memset(mOuter,0,sizeof(mOuter));
   start_time = time(NULL);	 
   mChoice = NULL;

}
PortStat::~PortStat()
{
	Cleanup();
#ifdef HAVE_SQLITE
  if(db) delete(db);
#endif
}
void PortStat::Cleanup()
{
  Trace::traceEvent(TRACE_NORMAL, "memory cleanup!");  
  if( mInputer ) {
	  delete mInputer;
	  mInputer = NULL;
  }
  for(int i=0;i<MAX_OUTPUT;i++){
	  if( mOuter[i] ){
		  delete mOuter[i];
		  mOuter[i] = NULL;
	  }
  }
  if( mChoice ){
	  delete mChoice;
	  mChoice = NULL;
  }
}
void PortStat::SetPrefs(Prefs *_prefs) {

}


void PortStat::Daemonize() {
#ifndef WIN32
  int childpid;

  signal(SIGHUP, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);

  if((childpid = fork()) < 0)
    Trace::traceEvent(TRACE_ERROR, "Occurred while daemonizing (errno=%d)",
				 errno);
  else {
    if(!childpid) { /* child */
      int rc;

      //Trace::traceEvent(TRACE_NORMAL, "Bye bye: I'm becoming a daemon...");

#if 1
      rc = chdir("/");
      if(rc != 0)
	Trace::traceEvent(TRACE_ERROR, "Error while moving to / directory");

      setsid();  /* detach from the terminal */

      fclose(stdin);
      fclose(stdout);
      /* fclose(stderr); */

      /*
       * clear any inherited file mode creation mask
       */
      umask(0);

      /*
       * Use line buffered stdout
       */
      /* setlinebuf (stdout); */
      setvbuf(stdout, (char *)NULL, _IOLBF, 0);
#endif
    } else { /* father */
      Trace::traceEvent(TRACE_NORMAL,
				   "Parent process is exiting (this is normal)");
      exit(0);
    }
  }
#endif
}

static bool isNumber(const char *str) {
  while(*str) {
    if(!isdigit(*str))
      return(false);

    str++;
  }

  return(true);
}
BOOL PortStat::Create()
{
	mName ="PortStat";
	const char * name = gPrefs->get_if_name();
	CAdapterFinder finder;
	finder.Find();
	char ip[20];
	if(name == NULL) {
		Trace::traceEvent(TRACE_WARNING, "No capture interface specified");
		finder.PrintAdapter(stdout);
		name = finder.AdapterName(1);
		if(name == NULL) {
			Trace::traceEvent(TRACE_ERROR, "Unable to locate default interface " );
			return FALSE;
		}
		strcpy(ip,finder.AdapterIp(1));
	} else {
		if(isNumber(name)) {
			int id = atoi(name);
			name = finder.AdapterName(id);
			if( name == NULL ){
				Trace::traceEvent(TRACE_WARNING, "Unable to locate interface Id %d", id);
				finder.PrintAdapter(stdout);
				return FALSE;
			}
			strcpy(ip,finder.AdapterIp(id));
		}
	}
	mChoice = new DataSwitch();
	mInputer = new PcapInput(name);
	mInputer->SetOutputer(this);
	finder.Free();

	if( !mInputer->Create() ) return FALSE;

	if(gPrefs->get_packet_filter() != NULL)
	  ((PcapInput *)mInputer)->SetFilter(gPrefs->get_packet_filter());

	Trace::traceEvent(TRACE_NORMAL, "Registered interface %s", mInputer->Name());
	
	if( gPrefs->get_output_mode() & 0x01 )	mOuter[0] = new FileWriter(mChoice);

	if( gPrefs->get_output_mode() & 0x02 ){
		if( gPrefs->get_local_network() )	mOuter[1] = new GatherOuter( gPrefs->get_local_network() );
		else mOuter[1] = new GatherOuter(ip);
	}
	if( gPrefs->get_output_mode() & 0x04 ){
		if( gPrefs->get_local_network() )	mOuter[2] = new RawOuter( gPrefs->get_local_network(),mChoice);
		else mOuter[2] = new RawOuter(ip,mChoice);
	}
	if(mOuter[0]) mOuter[0]->Create();
	if( mOuter[1] ) return mOuter[1]->Create();
	if( mOuter[2] ) return mOuter[2]->Create();
	return 1;
}

void PortStat::Write(NetPacket & packet)
{
  for(int i=0;i<MAX_OUTPUT;i++){
	  if( mOuter[i] )	  mOuter[i]->Write(packet);
  }
}
#ifndef HAVE_PTHREAD_H
void PortStat::doIdle()
{
  for(int i=0;i<MAX_OUTPUT;i++){
	  if( mOuter[i] )	  mOuter[i]->doIdle();
  }
}
#endif
BOOL PortStat::Run()
{
  for(int i=0;i<MAX_OUTPUT;i++){
	  if( mOuter[i] ){
		  mOuter[i]->Run();
	  }
  }
	mInputer->StartPolling();
	return TRUE;
}
void PortStat::Close() {
  if(mInputer) {
	  if( mInputer->isRunning() ){
		mInputer->Shutdown();
		Trace::traceEvent(TRACE_NORMAL, "Interface %s [running: %d]",
					 mInputer->Name(), mInputer->getStatus());
		return ;
	  }
  }
  for(int i=0;i<MAX_OUTPUT;i++){
	  if( mOuter[i] ){
		  mOuter[i]->Close();
	  }
  }
}
