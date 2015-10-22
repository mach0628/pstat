#ifndef __ROLLING_FILE_H___
#define __ROLLING_FILE_H___

class CRollingFile
{
	FILE * mFile;
	const char * mNameFormat;
	time_t mLast;
public:
	CRollingFile(void);
	virtual ~CRollingFile(void);

	int open(const char * fmt);
	void close();
	void checkAndRoll(time_t a);
	FILE * Handle(){ return mFile; };
};

#endif

