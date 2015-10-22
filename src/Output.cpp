#include "stdafx.h"
#include "Output.h"
#include "GatherLine.h"
#ifdef HAVE_PTHREAD_H
#include "pthread.h"
#endif
#include "Trace.h"
#include "Utils.h"
#include "Prefs.h"

FileWriter::FileWriter(DataSwitch * choice)
{
	mName="FileWriter";
	mblogFile = 0;
	mChoice = choice;
}
FileWriter::~FileWriter()
{
	
}
BOOL FileWriter::Create()
{
	if( gPrefs->isDaemonize() ){
		char path[MAX_PATH];
		snprintf(path, sizeof(path), "%s/packet.log", gPrefs->get_data_dir());
		Utils::fixPath(path);
		logFd = fopen(path,"a");
		if( !logFd ){
			Trace::traceEvent(TRACE_ERROR, "Unable to open log file on %s : please specify a different directory (-d)", path);
			return FALSE;
		}
		mblogFile =1;
	} else 	logFd = stdout;
	return TRUE;
}
BOOL FileWriter::Run()
{
	set_status_run();
	return TRUE;
}
void FileWriter::Close()
{
	if( isRunning() ){
		set_status_down();
		Trace::traceEvent(TRACE_NORMAL,"FileWriter outer  down!");
		if( mblogFile && (logFd!=NULL) ) fclose(logFd);
		logFd =NULL;
	}
}
void FileWriter::Write(NetPacket & packet)
{
	static int lastupdate=0;
	struct tm ltime;
    char timestr[16];
    time_t local_tv_sec;
	if( (packet.length -packet.off_data)>5 ) {
		if( !mChoice->CanSave(packet) ) return;
		/* 将时间戳转换成可识别的格式 */
		local_tv_sec =packet.tv_sec;
		//ltime=localtime(&local_tv_sec);
		localtime_r(&local_tv_sec,&ltime);
		strftime( timestr, sizeof timestr, "%H:%M:%S", &ltime);
		/* 打印数据包的时间戳和长度 */
		fprintf(logFd,"%s len: %d ", timestr,packet.length);
		if( packet.ip_proto==1 ) fprintf(logFd,"TCP %d.%d.%d.%d %d -> %d.%d.%d.%d %d %d\n",  packet.src_b[0], packet.src_b[1], packet.src_b[2], 
		   packet.src_b[3], packet.th_sport, packet.dst_b[0],  packet.dst_b[1], packet.dst_b[2],  packet.dst_b[3],
		   packet.th_dport,packet.length -packet.off_data);
		else 
			fprintf(logFd,"UDP %d.%d.%d.%d %d -> %d.%d.%d.%d %d %d\n", packet.src_b[0], packet.src_b[1], packet.src_b[2], 
		   packet.src_b[3], packet.th_sport, packet.dst_b[0],  packet.dst_b[1], packet.dst_b[2],  packet.dst_b[3],
		   packet.th_dport ,packet.length -packet.off_data);
		lastupdate++;
		if( lastupdate>60 ){
			fflush(logFd);
			lastupdate=0;
		}

	}
}
RawOuter::RawOuter(char * my,DataSwitch * choice)
{
	mName="RawOuter";
	mLocal = inet_addr(my);
	Trace::traceEvent(TRACE_NORMAL,"Raw outer set ip : %s",my);
	Trace::traceEvent(TRACE_NORMAL,"get ride of line is  %d",gPrefs->rid_line );	
	mblogFile = 0;
	mChoice = choice;
}
RawOuter::~RawOuter()
{

}
BOOL RawOuter::Create()
{
	if( gPrefs->isDaemonize() ){
		if( !logFile.open("%s/updata%d-%02d-%02d.log") ) return FALSE;
		mblogFile = 1;
	} 

	return TRUE;
}
BOOL RawOuter::Run()
{
	set_status_run();
	return TRUE;
}
void RawOuter::Close()
{
	if( isRunning() ){
		set_status_down();
		Trace::traceEvent(TRACE_NORMAL,"Raw outer  down!");
		if( mblogFile ) logFile.close();
	}
}
void RawOuter::Write(NetPacket & packet)
{
	static int lastupdate=0;
	struct tm ltime;
    char timestr[32];
    time_t local_tv_sec;
	int len = packet.length-packet.off_data;
	if( len<10 ) return;
	if( packet.ip_proto!=1 ) return;
	if( packet.ip_dst == mLocal ){
		//if( packet.th_dport<80 ) return;
		if( !mChoice->CanSave(packet) ) return;
		/* 将时间戳转换成可识别的格式 */
		local_tv_sec =packet.tv_sec;

		FILE * fd ;
		if( mblogFile ) {
			//logFile.checkAndRoll(local_tv_sec);
			logFile.checkAndRoll(time(NULL));
			fd = logFile.Handle();
		} else fd = stdout;

		//ltime=localtime(&local_tv_sec);
		localtime_r(&local_tv_sec,&ltime);
		sprintf(timestr,"%02d-%02d %02d:%02d:%02d",ltime.tm_mon+1,ltime.tm_mday,ltime.tm_hour,ltime.tm_min,ltime.tm_sec);
		/* 打印数据包的时间戳和长度 */

		fprintf(fd,"%s %d.%d.%d.%d %d -> %d (%d) \"", timestr, packet.src_b[0], packet.src_b[1], packet.src_b[2], 
			packet.src_b[3], packet.th_sport, packet.th_dport,len);
		if( gPrefs->rid_line ){
			char * tmp =(char *)packet.data;
			for(int i=0;i<packet.len_data;i++){
				if( tmp[i]==0x0d ) tmp[i]='!';
				if( tmp[i]==0x0a ) tmp[i]='$';
			}
		}
		fwrite(packet.data,1,packet.len_data,fd);
		fprintf(fd,"\"\n");
		lastupdate++;
		if( lastupdate>3 ){
			fflush(fd);
			lastupdate=0;
		}
	}
}
GatherOuter::GatherOuter(char * my)
{
	mName="GatherOuter";
	mLocal = inet_addr(my);
	mData =NULL;
	mSaveData =NULL;
	
	mblogFile = 0;
//	logFd = NULL;
#ifdef HAVE_PTHREAD_H
	mTimer = NULL;
#endif
	Trace::traceEvent(TRACE_NORMAL,"Gather outer set ip : %s",my);
}
GatherOuter::~GatherOuter()
{
	if( mData ) delete mData;
	if( mSaveData) delete mSaveData;

}
void * thread_proc(void * parm)
{
	while( ((GatherOuter *)parm)->Check() ) sleep(1);
	Trace::traceEvent(TRACE_NORMAL,"Gather outer thread down");
	return 0;
}
BOOL GatherOuter::Create()
{
	if( gPrefs->isDaemonize() ){
		if( !logFile.open("%s/gather%d-%02d-%02d.log") ) return FALSE;
		mblogFile = 1;
	} //else 	logFd = stdout;
	mData = new CGatherLine();
	Trace::traceEvent(TRACE_NORMAL,"Gather recorder unit is %d",sizeof(StatCell));

	return TRUE;
}
BOOL GatherOuter::Run()
{
	set_status_run();
	Trace::traceEvent(TRACE_NORMAL,"Gather start thread...");

#ifdef HAVE_PTHREAD_H
	if( pthread_create((pthread_t *)&mTimer,NULL,thread_proc,this) ) return FALSE;
#endif
	return TRUE;
}
void GatherOuter::Close()
{
	if( isRunning() ) {
		void *res;
		Trace::traceEvent(TRACE_NORMAL,"Gather outer sign...");
#ifdef HAVE_PTHREAD_H
		set_status_signed();
		pthread_join((pthread_t )mTimer,&res);
		sleep(2);
#else
		set_status_down();
		mSaveData = mData;
		mData = NULL;
		RemoveLine();
		if( mblogFile ) logFile.close();
#endif
	}
}
void GatherOuter::RemoveLine()
{
	if( !mSaveData ) return ;
	Trace::traceEvent(TRACE_NORMAL,"Gather save data[%d]...",mSaveData->GetLength());
	StatCell * p = mSaveData->GetHead();
	char buff[32];
	memset(buff,0,sizeof(buff));
	Utils::formattime2(buff,mSaveData->getStart());
	if( mblogFile ) logFile.checkAndRoll(mSaveData->getStart());
	FILE * fd = mblogFile?logFile.Handle():stdout;
	for(int i=0;i<mSaveData->GetLength();i++){		 
		//124.65.136.230 - - [23/Jun/2014:09:41:07 +0800] "GET /calendar/skin/default/img.gif HTTP/1.1" 200 1578
		char * p2 = Utils::iptos(p[i].ip);
		//fprintf(logFd,"%s %s %u up %d down %d\n",buff,Utils::iptos(p[i].ip),p[i].port,p[i].up,p[i].down);
		if( p[i].in ) fprintf(fd,"%s - - [%s] \"GET /in/%u/%s:%u HTTP/1.1\" 200 %d\n",p2,buff,p[i].lport,p2,p[i].port,p[i].in);
		if( p[i].out ) fprintf(fd,"%s - - [%s] \"GET /out/%u/%s:%u HTTP/1.1\" 200 %d\n",p2,buff,p[i].lport,p2,p[i].port,p[i].out);
	}
	fflush(fd);
	delete mSaveData;
	mSaveData =NULL;
	
}

int GatherOuter::Check()
{
	if( isDown() ){
		RemoveLine();
		mSaveData = mData;
		mData = NULL;
		RemoveLine();
		set_status_down();
		if( mblogFile ) logFile.close();
		return 0 ;
	}
	if( mData ){

		if( mData->nextLine() ) {
			mSaveData = mData;
			mSaveSnap =0;
			mData = new CGatherLine();
		}
	}
	if( mSaveData ){
		mSaveSnap++;
		if( mSaveSnap>3 ) 	RemoveLine();
	}
	return 1;
}
#ifndef HAVE_PTHREAD_H
void GatherOuter::doIdle()
{
	if( mData->nextLine() ) {
		mSaveData = mData;
		mData = new CGatherLine();
		RemoveLine();
	}
}
#endif
void GatherOuter::Write(NetPacket & packet)
{
	int len = packet.length-packet.off_data;
	if( len<1 ) return ;
#ifndef HAVE_PTHREAD_H
	if( mData==NULL ) mData = new CGatherLine();
	if( mData->nextLine() ) {
		mSaveData = mData;
		mData = new CGatherLine();
		RemoveLine();
	}
#endif
    CGatherLine * p = mData;
	if( packet.ip_dst == mLocal ){
		//Trace::traceEvent(TRACE_NORMAL,"Gather save in data[%d]...",len);
		p->update(packet.ip_src,packet.th_sport,packet.th_dport,1,len);
		p->update(mLocal,packet.th_dport,0,1,len);
	}
	if( packet.ip_src == mLocal ) {
		//Trace::traceEvent(TRACE_NORMAL,"Gather save out data[%d]...",len);
		p->update(packet.ip_dst,packet.th_dport,packet.th_sport,0,len);
		p->update(mLocal,packet.th_sport,0,0,len);
	}
}

