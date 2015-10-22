#ifndef __MONITOR_H__
#define  __MONITOR_H__

#include "Packet.h"
typedef	enum Stater_Status {
		StaterNew=0,
		StaterRun,
		StaterSigned,
		StaterDown
	} STATER_STATUS;
class DataSwitch
{

	u_int32_t * iplist;
	int maxip;
	int maxport;
public:
	DataSwitch();
	~DataSwitch();
public:
	int CanSave(NetPacket & packet);
};
class NetOutputer
{
	int mStatus;
protected:
	char * mName ;

protected:

public:
	NetOutputer();
	virtual ~NetOutputer();
public:
	int getStatus(){ return mStatus;};
	inline BOOL  isRunning() { return mStatus==StaterRun; };	
	inline BOOL  isDown() { return mStatus!=StaterRun; };
	inline BOOL  isSigned() { return mStatus== StaterSigned; };

	inline void  set_status_run(){ mStatus=StaterRun; };
	inline void  set_status_signed() { mStatus = StaterSigned; };
	inline void  set_status_down() { mStatus = StaterDown; };

	inline char* Name()              { return(mName);   }; 
	virtual BOOL Create()=0;
	virtual BOOL Run()=0;
	virtual void Close()=0;
#ifndef HAVE_PTHREAD_H
	virtual void doIdle(){};
#endif
	virtual void Write(NetPacket & packet)=0;
};


class NetInputer{
	int  mStatus;
protected:
	char * mName ;
	HANDLE mHandle;
	NetOutputer * mWriter;
public:
	NetInputer(const char * name);
	virtual ~NetInputer();
public:
	int getStatus(){ return mStatus;};
	inline BOOL  isRunning() { return mStatus==StaterRun; };
	inline void  set_status_run(){ mStatus=StaterRun; };
	
	inline BOOL  isDown() { return mStatus!=StaterRun; };
	inline BOOL  isSigned() { return mStatus== StaterSigned; };

	inline void  set_status_signed() { mStatus = StaterSigned; };
	inline void  set_status_down() { mStatus = StaterDown; };

	inline char* Name()              { return(mName);   }; 

	inline void SetOutputer(NetOutputer * out){ mWriter = out; };
	virtual BOOL Create();
	virtual void StartPolling();
	virtual void Shutdown();
};



#endif


