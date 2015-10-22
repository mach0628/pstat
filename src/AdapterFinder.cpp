#include "stdafx.h"

#include "AdapterFinder.h"

#ifndef WIN32
#include <pcap.h>
#else
#include <IPHlpApi.h>
#pragma comment(lib,"Iphlpapi.lib") 
#endif

CAdapterFinder::CAdapterFinder(void)
{
		m_pAdapterInfo=NULL;
}

CAdapterFinder::~CAdapterFinder(void)
{
	Free();
}
#ifndef WIN32
void CAdapterFinder::Free()
{
	if( m_pAdapterInfo ) pcap_freealldevs((pcap_if_t *)m_pAdapterInfo);
	m_pAdapterInfo=NULL;
}
BOOL CAdapterFinder::Find()
{
	if( m_pAdapterInfo ) pcap_freealldevs((pcap_if_t *)m_pAdapterInfo);
	char errbuf[PCAP_ERRBUF_SIZE];
	int i=pcap_findalldevs((pcap_if_t **)&m_pAdapterInfo,errbuf);
	if( i==-1){
		fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
		return FALSE;
	}
	return TRUE;
	//return pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf);
}
const char * CAdapterFinder::AdapterName(int index)
{
	pcap_if_t *d;
	int i =1;
	/* Jump to the selected adapter */
	for(d=(pcap_if_t *)m_pAdapterInfo; d ;d=d->next, i++) 
		if( index==i ) {
			return d->name;
		}
	return NULL;
}
const char * CAdapterFinder::AdapterDesc(int index)
{
	pcap_if_t *d;
	int i=1;
	/* Jump to the selected adapter */
	for(d=(pcap_if_t *)m_pAdapterInfo; d ;d=d->next, i++) 
		if( index==i ) return d->description;
	return NULL;
}
int CAdapterFinder::PrintAdapter(FILE *out)
{
	pcap_if_t *d;
	int i=0;
	for(d=(pcap_if_t *)m_pAdapterInfo; d; d=d->next)
	{
		fprintf(out,"%d. %s", ++i, d->name);
		if (d->description)
			fprintf(out," (%s)\n", d->description);
		else
			fprintf(out," (No description available)\n");
	}
	return i;
}
/* From tcptraceroute, convert a numeric IP address to a string */
#define IPTOSBUFFERS	12
static char *iptos(u_long in)
{
	static char output[IPTOSBUFFERS][3*4+3+1];
	static short which;
	u_char *p;

	p = (u_char *)&in;
	which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
	sprintf(output[which], "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
	return output[which];
}

static char* ip6tos(struct sockaddr *sockaddr, char *address, int addrlen)
{
	socklen_t sockaddrlen;

	#ifdef WIN32
	sockaddrlen = sizeof(struct sockaddr_in6);
	#else
	sockaddrlen = sizeof(struct sockaddr_storage);
	#endif


	if(getnameinfo(sockaddr, 
		sockaddrlen, 
		address, 
		addrlen, 
		NULL, 
		0, 
		NI_NUMERICHOST) != 0) address = NULL;

	return address;
}
const char * CAdapterFinder::AdapterIp(int index)
{
	pcap_if_t *d;
	pcap_addr_t *a;
	int i =1;
	/* Jump to the selected adapter */
	for(d=(pcap_if_t *)m_pAdapterInfo; d ;d=d->next, i++) 
		if( index==i ) {
			for(a=d->addresses;a;a=a->next) {
				if( a->addr->sa_family == AF_INET ) return iptos( ((struct sockaddr_in *)a->addr)->sin_addr.s_addr );
			}
		}
	return 0;
}
int CAdapterFinder::PrintAdapterLong(FILE *out)
{
  pcap_addr_t *a;
  char ip6str[128];
	pcap_if_t *d;
	int i=0;
	for(d=(pcap_if_t *)m_pAdapterInfo; d; d=d->next)
	{
	  /* Name */
	  printf("%d. %s", ++i, d->name);

	  /* Description */
	  if (d->description)
		printf("\tDescription: %s\n",d->description);

	  /* Loopback Address*/
	  printf("\tLoopback: %s\n",(d->flags & PCAP_IF_LOOPBACK)?"yes":"no");

	  /* IP addresses */
	  for(a=d->addresses;a;a=a->next) {
		printf("\tAddress Family: #%d\n",a->addr->sa_family);
	  
		switch(a->addr->sa_family)
		{
		  case AF_INET:
			printf("\tAddress Family Name: AF_INET\n");
			if (a->addr)
			  printf("\tAddress: %s\n",iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr));
			if (a->netmask)
			  printf("\tNetmask: %s\n",iptos(((struct sockaddr_in *)a->netmask)->sin_addr.s_addr));
			if (a->broadaddr)
			  printf("\tBroadcast Address: %s\n",iptos(((struct sockaddr_in *)a->broadaddr)->sin_addr.s_addr));
			if (a->dstaddr)
			  printf("\tDestination Address: %s\n",iptos(((struct sockaddr_in *)a->dstaddr)->sin_addr.s_addr));
			break;

		  case AF_INET6:
		   printf("\tAddress Family Name: AF_INET6\n");
	#ifndef __MINGW32__ /* Cygnus doesn't have IPv6 */
			if (a->addr)
			  printf("\tAddress: %s\n", ip6tos(a->addr, ip6str, sizeof(ip6str)));
	#endif
			break;

		  default:
			printf("\tAddress Family Name: Unknown\n");
			break;
		}
	  }
	  printf("\n");
	}
	return i;
}
#else
void CAdapterFinder::Free()
{
	if(m_pAdapterInfo ) free(m_pAdapterInfo);
	m_pAdapterInfo =NULL;
}

BOOL CAdapterFinder::Find()
{
	Free();
	//PIP_ADAPTER_INFO m_pAdapterInfo; 
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO); 
	m_pAdapterInfo = malloc(ulOutBufLen); 
	DWORD dwRetVal = GetAdaptersInfo( (PIP_ADAPTER_INFO)m_pAdapterInfo, &ulOutBufLen);
	// 第一次调用GetAdapterInfo获取ulOutBufLen大小 
	if (dwRetVal == ERROR_BUFFER_OVERFLOW) 
	{ 
		free(m_pAdapterInfo); 
		m_pAdapterInfo = malloc (ulOutBufLen); 
		dwRetVal = GetAdaptersInfo( (PIP_ADAPTER_INFO)m_pAdapterInfo, &ulOutBufLen);
	} 
	if (dwRetVal == NO_ERROR){
		PIP_ADAPTER_INFO d;
		for(d=(PIP_ADAPTER_INFO)m_pAdapterInfo; d ;d=d->Next) {
			memmove(d->AdapterName+12,d->AdapterName,strlen(d->AdapterName)+1);
		    memcpy(d->AdapterName,"\\Device\\NPF_",12);
		}
		return TRUE;
	}
	Free();
	return FALSE;
}
int CAdapterFinder::PrintAdapter(FILE * out)
{ 
	PIP_ADAPTER_INFO pAdapter = (PIP_ADAPTER_INFO)m_pAdapterInfo; 
	int i=0;
	while (pAdapter) { 
		fprintf(out,"%d. %s", ++i, pAdapter->AdapterName);
		if (pAdapter->Description)
			fprintf(out," (%s)\n", pAdapter->Description);
		else
			fprintf(out," (No description available)\n");
		pAdapter = pAdapter->Next; 
	}// end while 
	return i;
}

int CAdapterFinder::PrintAdapterLong(FILE * out)
{
	return PrintAdapter(out);
}
const char * CAdapterFinder::AdapterIp(int index)
{
	PIP_ADAPTER_INFO d;
	int i=1;
	for(d=(PIP_ADAPTER_INFO)m_pAdapterInfo; d ;d=d->Next, i++) 
		if( index==i ) {
			return (d->IpAddressList.IpAddress.String);
		}
	return NULL;
}
const char * CAdapterFinder::AdapterName(int index)
{
	PIP_ADAPTER_INFO d;
	int i=1;
	for(d=(PIP_ADAPTER_INFO)m_pAdapterInfo; d ;d=d->Next, i++) 
		if( index==i ) {
			return d->AdapterName;
		}
	return NULL;
}
const char * CAdapterFinder::AdapterDesc(int index)
{
	PIP_ADAPTER_INFO d; 
	int i=1;
	for(d=(PIP_ADAPTER_INFO)m_pAdapterInfo; d ;d=d->Next, i++) 
		if( index==i ) {
			return d->Description;
		}
	return NULL;
}
#endif

