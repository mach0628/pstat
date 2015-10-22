/* @(#) $Header: /tcpdump/master/tcpdump/ether.h,v 1.8 2002/12/11 07:13:51 guy Exp $ (LBL) */
/*
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)if_ether.h	8.3 (Berkeley) 5/2/95
 */
#ifndef __ETHER_H__
#define __ETHER_H__
#define	ETHERMTU	1500

/* Can't add any fields not in the real header here 
   because of how the decoder uses structure overlaying */
#ifdef WIN32
  /* Visual C++ pragma to disable warning messages about nonstandard bit field type */
  #pragma warning( disable : 4214 )  
#pragma pack(push)
#pragma pack(1)
#endif
/*
 * The number of bytes in an ethernet (MAC) address.
 */
#define	ETHER_ADDR_LEN		6

/*
 * Structure of a DEC/Intel/Xerox or 802.3 Ethernet header.
 */
/* 
 * Ethernet header
 */

typedef struct _EtherHdr
{
    u_int8_t ether_dst[6];
    u_int8_t ether_src[6];
    u_int16_t ether_type;

}  EtherHdr;


/*
 * Length of a DEC/Intel/Xerox or 802.3 Ethernet header; note that some
 * compilers may pad "struct ether_header" to a multiple of 4 bytes,
 * for example, so "sizeof (struct ether_header)" may not give the right
 * answer.
 */
#define ETHERNET_HEADER_LEN		14


typedef struct _IPHdr
{
#if defined(WORDS_BIGENDIAN)
    u_int8_t ip_ver:4,  /* IP version */
    ip_hlen:4;          /* IP header length */
#else
    u_int8_t ip_hlen:4, ip_ver:4;
#endif
    u_int8_t ip_tos;        /* 服务类型(Type of service)  */
    u_int16_t ip_len;       /* 总长(Total length)*/
    u_int16_t ip_id;        /* identification  */
    u_int16_t ip_off;       /*标志位(Flags) (3 bits) + 段偏移量(Fragment offset) (13 bits) */
    u_int8_t ip_ttl;        /* time to live field */
    u_int8_t ip_proto;      /*  协议(Protocol)*/
    u_int16_t ip_csum;      /* checksum */
    u_int32_t ip_src;  /* source IP */
    u_int32_t ip_dst;  /* dest IP */
}      IPHdr;
typedef struct _TCPHdr
{
    u_int16_t th_sport;     /* source port */
    u_int16_t th_dport;     /* destination port */
    u_int32_t th_seq;       /* sequence number */
    u_int32_t th_ack;       /* acknowledgement number */
#ifdef WORDS_BIGENDIAN
    u_int8_t th_off:4,      /* data offset */
    th_x2:4;       /* (unused) */
#else
    u_int8_t th_x2:4, th_off:4;
#endif
    u_int8_t th_flags;
    u_int16_t th_win;       /* window */
    u_int16_t th_sum;       /* checksum */
    u_int16_t th_urp;       /* urgent pointer */

}       TCPHdr;
/* UDP 首部*/
typedef struct _UDPHdr{
    u_short uh_sport;          // 源端口(Source port)
    u_short uh_dport;          // 目的端口(Destination port)
    u_short uh_len;            // UDP数据包长度(Datagram length)
    u_short uh_chk;            // 校验和(Checksum)
}UDPHdr;

#ifdef WIN32
  /* Visual C++ pragma to enable warning messages about nonstandard bit field type */
  #pragma warning( default : 4214 )
#pragma pack(pop)
#endif

#define IP_HEADER_LEN           20
#define TCP_HEADER_LEN          20
#define UDP_HEADER_LEN          8

#define ETHERNET_MTU            1500
#define ETHERNET_TYPE_IP        0x0800
#define ETHERNET_TYPE_ARP       0x0806
#define ETHERNET_TYPE_REVARP    0x8035
#define ETHERNET_TYPE_IPV6      0x86dd
#define ETHERNET_TYPE_IPX        0x8137
#define ETHERNET_TYPE_PPPoE_DISC    0x8863 /* discovery stage */
#define ETHERNET_TYPE_PPPoE_SESS    0x8864 /* session stage */
#define ETHERNET_TYPE_8021Q     0x8100
#define ETHERNET_TYPE_8023Q     0x8023

#endif
