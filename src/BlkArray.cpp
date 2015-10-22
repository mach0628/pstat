#include "stdafx.h"
#include "BlkArray.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

CBlkArray::CBlkArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_Length=0;
	m_BlkSize =1;
}
CBlkArray::CBlkArray(int blk)
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_Length =0;
	m_BlkSize =blk;
}
CBlkArray::~CBlkArray(void)
{
	if( m_pData  ) delete m_pData;
}
void CBlkArray::SetBlkSize(int blk)
{
	m_BlkSize =blk;
	if( m_pData ) delete m_pData;
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_Length = 0;

}
void CBlkArray::SetSize(int nNewSize)
{
	if (nNewSize == 0)	{
		// shrink to nothing
		if (m_pData != NULL) {
			delete[] m_pData;
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize =m_Length= 0;
	}	else	{
		if (m_pData == NULL){
			// create one with exact size
			m_pData = new u_int8_t[nNewSize];
			memset(m_pData,0,nNewSize);
			m_nSize = m_Length= 0;
			m_nMaxSize = nNewSize;
		}	else	{
			if (nNewSize <= m_nMaxSize)	{
				// it fits
				if (nNewSize > m_nSize)	{
					// initialize the new elements
					memset(&m_pData[m_nSize],0, (nNewSize-m_nSize));
				} else if (m_nSize > nNewSize)	{
					// destroy the old elements
					memset(&m_pData[nNewSize],0, (m_nSize-nNewSize));
				}
			}	else	{
				// heuristically determine growth when nGrowBy == 0
				//  (this avoids heap fragmentation in many situations)
				int nGrowBy = m_nSize / 8;
				nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
				int nNewMax;
				if (nNewSize < m_nMaxSize + nGrowBy)
					nNewMax = m_nMaxSize + nGrowBy;  // granularity
				else
					nNewMax = nNewSize;  // no slush

				u_int8_t* pNewData = new u_int8_t[nNewMax ];

				// copy new data from old
				memcpy(pNewData, m_pData, m_nSize );

				// construct remaining elements
			//	ASSERT(nNewSize > m_nSize);
				memset(&pNewData[m_nSize], 0,(nNewSize-m_nSize));

				// get rid of old stuff (note: no destructors called)
				delete[] m_pData;
				m_pData = pNewData;
				m_nMaxSize = nNewMax;
			}
		}
	}
}
u_int8_t * CBlkArray::Add(void * newElement)
{ 	
	if( (m_nSize+m_BlkSize)>=m_nMaxSize ) SetSize(m_nSize+m_BlkSize*4);
	memcpy(m_pData+m_nSize,newElement,m_BlkSize);
	m_nSize +=m_BlkSize;
	m_Length ++;
	return m_pData+(m_nSize-m_BlkSize);
}


void CBlkArray::SetAt(int nIndex, void * newElement)
{ 
	memcpy(m_pData+nIndex*m_BlkSize,newElement,m_BlkSize);
}


void CBlkArray::RemoveAt(int nIndex, int nCount)
{
	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);

	if (nMoveCount)
		memmove(m_pData+nIndex*m_BlkSize, m_pData+(nIndex + nCount)*m_BlkSize,
			nMoveCount * m_BlkSize);
	m_nSize -= nCount*m_BlkSize;
	m_Length -= nCount;
}

