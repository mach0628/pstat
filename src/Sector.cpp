#include "stdafx.h"
#include "Sector.h"
#include <pcap.h>
#include "ether.h"
#include "Trace.h"

CSector::CSector(void)
{
}

CSector::~CSector(void)
{
}
BOOL CSector::dissector(NetPacket & packet,const u_int8_t *pkt, u_int32_t length, u_int32_t caplen)
{
	return ether(packet,pkt,length,caplen);
}

BOOL CSector::ether(NetPacket & packet, const u_int8_t  *pkt, u_int32_t length, u_int32_t caplen)
{
		u_int32_t ip_len;       /* length from the start of the ip hdr to the pkt end */
		u_int16_t hlen;             /* ip header length */

    /* do a little validation */
    if( caplen < ETHERNET_HEADER_LEN)
    {
		Trace::traceEvent( TRACE_DEBUG,"Captured data length < Ethernet header length! (%d bytes)", caplen);
        return 0;
    }
    /* lay the ethernet structure over the packet data */
    EtherHdr * eh = (EtherHdr *) pkt;
	if( ntohs(eh->ether_type)==ETHERNET_TYPE_IP ){
		pkt +=ETHERNET_HEADER_LEN;
		IPHdr * iph = (IPHdr *)( pkt);
		
		caplen -=ETHERNET_HEADER_LEN;
		packet.length = length-ETHERNET_HEADER_LEN;
		if(caplen < IP_HEADER_LEN){
			Trace::traceEvent( TRACE_DEBUG,"Captured data length < Ethernet header length! (%d bytes)", caplen);
			return 0;
		}
		if(iph->ip_ver != 4){
			Trace::traceEvent( TRACE_DEBUG,"[!] WARNING: Not IPv4 datagram! ([ver: 0x%x][len: 0x%x])", iph->ip_ver, iph->ip_len);
			return 0;
		}

		/* set the IP datagram length */
		ip_len = ntohs(iph->ip_len);

		/* set the IP header length */
		hlen = (iph->ip_hlen << 2);
		if (ip_len != caplen) {
        if (ip_len > caplen) {
           // if (pv.verbose_flag)
                 Trace::traceEvent( TRACE_DEBUG,"[!] WARNING: IP Len field is %d bytes bigger"
                              " than captured length. (ip.len: %lu, cap.len: %lu)",
                             ip_len - caplen, ip_len, caplen);
            ip_len = caplen;
        } 
    }

    if(ip_len < hlen)   {
		Trace::traceEvent( TRACE_DEBUG,"[!] WARNING: IP dgm len (%d bytes) < IP hdr len (%d bytes), packet discarded", ip_len, hlen);
        return 0;
    }
	packet.ip_dst = iph->ip_dst;
	packet.ip_src = iph->ip_src;
	packet.package  = (const u_char *)iph;
	packet.len_pack = ip_len;
	packet.off_app = hlen;
    ip_len -= hlen;
		pkt += hlen;
        switch(iph->ip_proto)
        {
            case IPPROTO_TCP:
				{
					packet.ip_proto = PACKET_TCP;
                TCPHdr * tcph = (TCPHdr *) pkt;
				if(ip_len < 20) break;
				/* multiply the payload offset value by 4 */
				hlen = (tcph->th_off << 2);
				packet.th_sport= htons(tcph->th_sport);
				packet.th_dport = htons(tcph->th_dport);
				if(hlen < 20)  {
					Trace::traceEvent( TRACE_DEBUG,"[!] WARNING: TCP head len %d too small, ip head %d %d %d",hlen,packet.off_app,packet.th_sport,packet.th_dport);
					hlen = 20;
				}
   
				/* set the data pointer and size */
				pkt= (pkt + hlen);
				if(hlen < ip_len)    ip_len -=hlen;
				   else     ip_len = 0;
				   packet.data = pkt;
				   packet.off_data = hlen+packet.off_app;
				   packet.len_data = ip_len;
				   //printf("TCP %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d L[%d]\n",  iph->ip_src.addr_b.byte1, iph->ip_src.addr_b.byte2, iph->ip_src.addr_b.byte3, 
					  // iph->ip_src.addr_b.byte4, sport, iph->ip_dst.addr_b.byte1,  iph->ip_dst.addr_b.byte2, iph->ip_dst.addr_b.byte3,  iph->ip_dst.addr_b.byte4,
					  // dport,ip_len);
				   return 1;
				}
				break;
            case IPPROTO_UDP:
				{
					packet.ip_proto = PACKET_UDP;
				UDPHdr *udph = (UDPHdr *) pkt;
				packet.th_sport = htons(udph->uh_sport) ;
				packet.th_dport = htons(udph->uh_dport);
				if(ip_len > UDP_HEADER_LEN )
					 ip_len -=UDP_HEADER_LEN;
				else ip_len=0;
				pkt +=UDP_HEADER_LEN;	
				   packet.data = pkt;
				  packet.off_data = UDP_HEADER_LEN+packet.off_app;
				  packet.len_data = ip_len;
				   //printf("UDP %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d L[%d]\n",  iph->ip_src.addr_b.byte1, iph->ip_src.addr_b.byte2, iph->ip_src.addr_b.byte3, 
					  // iph->ip_src.addr_b.byte4, sport, iph->ip_dst.addr_b.byte1,  iph->ip_dst.addr_b.byte2, iph->ip_dst.addr_b.byte3,  iph->ip_dst.addr_b.byte4,
					  // dport,ip_len);
				   return 1;
				}
				break;
			default:
				//printf("OTHER \n");
				packet.off_data = packet.off_app;
				packet.data =pkt;
				break;
		}
	}
	return 0;
}


