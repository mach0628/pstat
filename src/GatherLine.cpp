#include "stdafx.h"
#include "GatherLine.h"
#include "Utils.h"
#include "Trace.h"

CGatherLine::CGatherLine()
{
	mData.SetBlkSize(sizeof(StatCell));
	reset();
}

CGatherLine::~CGatherLine(void)
{
	mData.RemoveAll();
}
#define CONST_MAX_ACTIVITY_DURATION 60
void CGatherLine::reset()
{
	mData.RemoveAll();
	mStart = time(NULL);
	mStart -= (mStart % CONST_MAX_ACTIVITY_DURATION);
	mStart += CONST_MAX_ACTIVITY_DURATION;
	char buff[32];
	Utils::formattime(buff,mStart);
	Trace::traceEvent(TRACE_INFO, "create  gather line  at %s", buff);
	
}
time_t CGatherLine::getStart()
{ 
	return mStart-CONST_MAX_ACTIVITY_DURATION;
}

int CGatherLine::GetLength()
{
	return mData.GetSize();
}
StatCell * CGatherLine::GetHead()
{
	return (StatCell *) mData.GetData();
}


int  CGatherLine::nextLine()
{
	time_t a;
	a = time(NULL);
	if( a>=mStart ) return 1;
	return 0;
}
StatCell * CGatherLine::FindCell(u_int32_t ip, u_int16_t port,u_int16_t lport)
{
	StatCell * p = ( StatCell * )mData.GetData();
	int i;
	for( i=0;i<mData.GetSize();i++){
		if( (p[i].ip ==ip)&&(p[i].port==port)&&(p[i].lport==lport) ) return &p[i];
	}
	StatCell m;
	memset(&m,0,sizeof(m));
	m.ip = ip;m.port=port;	m.lport = lport;
	return (StatCell *)mData.Add(&m);
}

void CGatherLine::update(u_int32_t ip, u_int16_t port, u_int16_t lport,int in, int length)
{
	StatCell * p = FindCell(ip,port,lport);
	if( in ) p->in += length;
		else p->out +=length;
}

