#ifndef __MINUTE_DATA_H__
#define __MINUTE_DATA_H__

#include "BlkArray.h"
typedef struct _StatCell{
	u_int32_t ip;
	u_int16_t port;
	u_int16_t lport;
	int in;
	int out;
} StatCell;
class CGatherLine
{
	time_t mStart;
	CBlkArray mData;
	int mRuning;
protected:
	StatCell * FindCell(u_int32_t ip,u_int16_t port,u_int16_t lport);
public:
	CGatherLine();
	~CGatherLine();
	void reset();
	int  nextLine();
	int  GetLength();
	StatCell * GetHead();
	time_t getStart();
	void update(u_int32_t ip,u_int16_t port,u_int16_t lport,int in,int length);
};

#endif

