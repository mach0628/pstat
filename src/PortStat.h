#ifndef __NSTAT_H__
#define __NSTAT_H__

#include "Prefs.h"
#include "Monitor.h"

#define MAX_OUTPUT 3
class PortStat: public NetOutputer{
private:

 DataSwitch * mChoice;
  NetInputer * mInputer;
  NetOutputer * mOuter[MAX_OUTPUT];
   Prefs * prefs;
public:
	PortStat();
	~PortStat();
protected:

public:
	time_t start_time;
	inline u_int Uptime()             { return((u_int)(time(NULL)-start_time+1)); };

public:
	void SetPrefs(Prefs *_prefs);

	virtual BOOL Create();
	virtual BOOL Run();
	virtual void Close();
#ifndef HAVE_PTHREAD_H
	virtual void doIdle();
#endif

	virtual void Write(NetPacket & packet);

	void Cleanup();
	void Daemonize();
  
};
extern PortStat* pstat;
#endif

