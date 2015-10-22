#include "stdafx.h"
#include "Monitor.h"
#include "Trace.h"
#include "Prefs.h"

DataSwitch::DataSwitch()
{
	iplist = NULL;
	maxip =0;maxport=0;
	
	Trace::traceEvent(TRACE_NORMAL,"ip list ,mode %d:",gPrefs->block_mode & 0x02);
	if( gPrefs->filter_ip[0]!=0 ){
		iplist = new u_int32_t[Prefs::MAX_FILTER];
		for( maxip=0;maxip<Prefs::MAX_FILTER;maxip++){
			if( gPrefs->filter_ip[maxip]==0 ) break;
			if( gPrefs->host_mode) iplist[maxip]=gPrefs->filter_ip[maxip]&0xFFFFFF00;
			else iplist[maxip]=gPrefs->filter_ip[maxip];
			Trace::traceEvent(TRACE_NORMAL,"add ip %d.%d.%d.%d",iplist[maxip]&0x000000FF,(iplist[maxip]&0x0000FF00)>8,
				(iplist[maxip]&0xFF000000)>16,(iplist[maxip]&0x00FF0000)>24);
		}
	}
	Trace::traceEvent(TRACE_NORMAL,"port list ,mode %d:",gPrefs->block_mode &0x01);
	for( maxport=0;maxport<Prefs::MAX_FILTER;maxport++){
		if( gPrefs->filter_port[maxport]==0 ) break;
		Trace::traceEvent(TRACE_NORMAL,"add port %u:",gPrefs->filter_port[maxport]);
	}

}
DataSwitch::~DataSwitch()
{
	if( iplist ) delete iplist;
}
int DataSwitch::CanSave(NetPacket &packet)
{
		if( maxip ){
			//block mode
			if( gPrefs->host_mode ){
				for(int i=0;i<maxip;i++)  if( (packet.ip_src&0xFFFFFF00) ==iplist[i] ) return 0;			
			} else {
				for(int i=0;i<maxip;i++)  if( (packet.ip_src) ==iplist[i] ) return 0;			
			}
		}
		if( maxport ){
			if( gPrefs->block_mode & 0x01 ){
				//block mode
				for(int i=0;i<maxport;i++)
					if( packet.th_dport==gPrefs->filter_port[i] ) return 0;
			} else{
				//pass mode,port in list to be record
				for(int i=0;i<maxport;i++)
					if( packet.th_dport==gPrefs->filter_port[i] ) return 1;
				return 0;
			}
		}
		return 1;
}

NetOutputer::NetOutputer()
{
	mName = NULL;
	mStatus = 0;


}
NetOutputer::~NetOutputer()
{
}


NetInputer::NetInputer(const char * name)
{
	if( name  ){
		mName = new char[strlen(name)+1];
		strcpy(mName,name );
	} else mName = NULL;
	mStatus = 0;
	mHandle = NULL;
	mWriter = NULL;
}
NetInputer::~NetInputer()
{
	if( mName ) delete mName;
}
BOOL NetInputer::Create( )
{
	return TRUE;
}
void NetInputer::StartPolling()
{
}
void NetInputer::Shutdown()
{
	set_status_down();
}

