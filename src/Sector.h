#ifndef __SECTOR_H__
#define __SECTOR_H__
#include "Packet.h"
class CSector
{
protected:
	static BOOL ether(NetPacket & packet, const u_int8_t  *pkt, u_int32_t length, u_int32_t caplen);
public:
	CSector(void);
	~CSector(void);
	static BOOL dissector(NetPacket & packet,const u_int8_t *pkt, u_int32_t length, u_int32_t caplen);
};

#endif
