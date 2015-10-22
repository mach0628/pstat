#include "stdafx.h"
#include "PcapInput.h"
#include <pcap.h>
#include "Prefs.h"
#include "Trace.h"
#include "Sector.h"

PcapInput::PcapInput(const char * name):NetInputer(name)
{
	mFromFile = FALSE;
}

PcapInput::~PcapInput(void)
{
  Shutdown();
  if(mHandle)  pcap_close((pcap_t *)mHandle);
}
BOOL PcapInput::Create( )
{
	
	char errbuf[PCAP_ERRBUF_SIZE];	
	
	if(!mWriter){
		 Trace::traceEvent(TRACE_NORMAL, "Not define outer!");
		 return FALSE;
	}
	Trace::traceEvent(TRACE_NORMAL, "Use outputer: %s",mWriter->Name());
	packets_captured = packets_discard= 0;
    mHandle= pcap_open_live(mName,	// name of the device
		gPrefs->getSnaplen(),			// portion of the packet to capture. 
										// 65536 grants that the whole packet will be captured on all the MACs.
						 0,				// promiscuous mode (nonzero means promiscuous)
						 500,			// read timeout
						 errbuf			// error buffer
						 );
	if( !mHandle ){
		Trace::traceEvent(TRACE_ERROR,"could not open pcap file: %s", errbuf);
		return FALSE;
	}
	Trace::traceEvent(TRACE_NORMAL, "Reading packets from interface %s...", mName);
	
	mType =  pcap_datalink((pcap_t *)mHandle);
	Trace::traceEvent(TRACE_NORMAL,"%s , data link[%s]",pcap_lib_version(),pcap_datalink_val_to_name(mType));
	//pcap_setbuff((pcap_t *)mHandle,6144000);
	return TRUE;
}

void PcapInput::StartPolling()
{
  Trace::traceEvent(TRACE_NORMAL, "Started packet polling on interface %s...", mName);
  set_status_run();
  
  pcap_t  *pd = (pcap_t *) mHandle;
  /* Wait until the initialization competes */
  while( isRunning() ) {
    const u_char *pkt;
    struct pcap_pkthdr hdr;
	NetPacket packet;

    if((pkt = pcap_next(pd, &hdr)) != NULL) {
		packets_captured++;
		if((hdr.caplen > 0) && (hdr.len > 0)){
			memset(&packet,0,sizeof(NetPacket));
			packet.tv_sec = hdr.ts.tv_sec;
			if( CSector::dissector(packet,pkt,hdr.len,hdr.caplen) )		mWriter->Write(packet);
			else packets_discard++;
		} else packets_discard++;
    } else {
      if(mFromFile)	break;
#ifndef HAVE_PTHREAD_H
	  mWriter->doIdle();
#endif
    }
  } /* while */
  set_status_down();
  Trace::traceEvent(TRACE_NORMAL, "Terminated packet polling for %s", mName);

}
void PcapInput::Shutdown()
{
 if( isRunning() ) {
    set_status_signed();
    printStat();
    if(mHandle) pcap_breakloop((pcap_t *)mHandle);
  }
}

int PcapInput::SetFilter(char *filter) 
{
  struct bpf_program fcode;
  struct in_addr netmask;

  netmask.s_addr = htonl(0xFFFFFF00);

  if((pcap_compile((pcap_t *)mHandle, &fcode, filter, 1, netmask.s_addr) < 0)
     || (pcap_setfilter((pcap_t *)mHandle, &fcode) < 0)) {
    Trace::traceEvent(TRACE_ERROR, "Unable to set filter %s. Filter ignored.", filter);
    return 0;
  } else {
    Trace::traceEvent(TRACE_NORMAL, "Packet capture filter set to \"%s\"", filter);
    return 1;
  }
}

void PcapInput::printStat()
{ 
	struct pcap_stat stat;
	if( pcap_stats((pcap_t *)mHandle,&stat)==0){
		//ps_ifdrop = stat.ps_ifdrop;	/* drops by interface XXX not yet supported */
		Trace::traceEvent(TRACE_NORMAL, "%u packets captured,  %u packets discard", packets_captured,packets_discard);
		Trace::traceEvent(TRACE_NORMAL, "%d packets received by filter", stat.ps_recv		/* number of packets received */);
		Trace::traceEvent(TRACE_NORMAL, "%d packets dropped by kernel", stat.ps_drop		/* number of packets dropped */);
		//Trace::traceEvent(TRACE_NORMAL, "%d packets dropped by interface", stat.ps_ifdrop	/* drops by interface XXX not yet supported */);
	}

}


