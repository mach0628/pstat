#ifndef __PCAP_INPUT_H__
#define __PCAP_INPUT_H__

#include "Monitor.h"

class PcapInput :public NetInputer
{
	int mType;
	int packets_captured;
	int packets_discard;
	int mFromFile;
	u_int32_t ps_recv;		/* number of packets received */
	u_int32_t ps_drop;		/* number of packets dropped */
	u_int32_t ps_ifdrop;	/* drops by interface XXX not yet supported */
public:
	PcapInput(const char * name );
	virtual ~PcapInput(void);
	
	int SetFilter(char * str);
	void printStat();

	virtual BOOL Create();
	virtual void StartPolling();
	virtual void Shutdown();
};

#endif

