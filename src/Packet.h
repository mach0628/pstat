#ifndef __PACKET_H__
#define __PACKET_H__


#define NETPACKET_HEAD (sizeof(NETPACKET)-8)
#define PACKET_TCP 1
#define PACKET_UDP 2
typedef struct _NetPacket{
	long    tv_sec;         /* seconds */
    u_int8_t ip_proto;      /* datagram protocol */
	u_int8_t  reserve;
	union{
    u_int32_t ip_src;  /* source IP */
	u_int8_t  src_b[4];
	};
	union{
    u_int32_t ip_dst;  /* dest IP */
	u_int8_t  dst_b[4];
	};
    u_int16_t th_sport;     /* source port */
    u_int16_t th_dport;     /* destination port */
	int    length; // ip包总长度，包括IP头＋协议头＋数据段
	u_int16_t off_app;//协议头偏移，也是IP头长度
	u_int16_t off_data; // 数据段偏移
	const u_int8_t * package; // ip包
	u_int32_t len_pack;
	const u_int8_t * data;// 数据段
	u_int32_t len_data;
} NetPacket;

#endif
