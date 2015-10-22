#pragma once

//#define HAVE_PCAP_FINDALLDEV

class CAdapterFinder
{
	HANDLE m_pAdapterInfo;
public:
	CAdapterFinder(void);
	~CAdapterFinder(void);
public:
	BOOL Find();
	void Free();
	int PrintAdapter(FILE * out);
	int PrintAdapterLong(FILE * out);
	const char * AdapterName(int index);
	const char * AdapterDesc(int index);
	const char * AdapterIp(int index);
};
