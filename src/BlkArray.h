#ifndef __BLOCK_ARRAY_H__
#define __BLOCK_ARRAY_H__

class CBlkArray
{
public:
	CBlkArray(int blk);
	CBlkArray();
	~CBlkArray(void);

protected:
	void SetSize(int nNewSize );

// Operations
	// Clean up
public:
	int GetSize() { return m_Length; };
	
	
	void SetBlkSize(int blk);
	void RemoveAll(){ SetSize(0); };

	// Direct Access to the element data (may return NULL)
	u_int8_t * GetData() const { return m_pData; };

	// Accessing elements
	
	void SetAt(int nIndex, void * newElement);

	u_int8_t * Add(void * newElement);

	void RemoveAt(int nIndex, int nCount = 1);

// Implementation
protected:
	u_int8_t * m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_Length;
	int m_BlkSize;
};


#endif




