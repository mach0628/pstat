#ifndef __NET_OUTPUT_H__
#define __NET_OUTPUT_H__
#include "Monitor.h"
#include "RollingFile.h"
class FileWriter:public NetOutputer
{
	FILE *logFd;
	int mblogFile;
	DataSwitch * mChoice;
public:
	FileWriter(DataSwitch * choice);
	virtual ~FileWriter();
public:
	virtual BOOL Create();
	virtual BOOL Run();
	virtual void Write(NetPacket & packet);
	virtual void Close();	
};

class CGatherLine;

class RawOuter:public NetOutputer
{
	CRollingFile logFile;
	int mblogFile;
	DataSwitch * mChoice;
	u_int32_t mLocal;
public:
	RawOuter(char * my,DataSwitch * choice);
	~RawOuter();
	virtual BOOL Create();
	virtual BOOL Run();
	virtual void Write(NetPacket & packet);
	virtual void Close();	
};
class GatherOuter:public NetOutputer
{
	//FILE *logFd;
	CRollingFile logFile;
	int mblogFile;
	CGatherLine * mData;
	CGatherLine * mSaveData;
	u_int32_t mLocal;
#ifdef HAVE_PTHREAD_H
	HANDLE mTimer;
	int mSaveSnap;
#endif
	void RemoveLine();
public:
	GatherOuter(char * my);
	~GatherOuter();
	virtual BOOL Create();
	virtual BOOL Run();
	virtual void Write(NetPacket & packet);
#ifndef HAVE_PTHREAD_H
	virtual void doIdle();
#else
		int Check();
#endif
	virtual void Close();	
};


#endif

