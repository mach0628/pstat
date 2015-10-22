#include "stdafx.h"
#include "RollingFile.h"
#include "Trace.h"
#include "Prefs.h"
#include "Utils.h"

CRollingFile::CRollingFile(void)
{
	mFile = NULL;
}

CRollingFile::~CRollingFile(void)
{
	if( mFile ) fclose(mFile);
}
void CRollingFile::close()
{
	if( mFile ) fclose(mFile);
	mFile = NULL;
}

#define CONST_MAX_ACTIVITY_DURATION (60*60*24)

void CRollingFile::checkAndRoll(time_t a)
{
	if( a>=mLast ) {
		if( mFile ) fclose(mFile);
		struct tm l;
		char path[MAX_PATH];
		localtime_r(&a,&l);
		snprintf(path, sizeof(path), mNameFormat, gPrefs->get_data_dir(),l.tm_year+1900,l.tm_mon+1,l.tm_mday,l.tm_hour);
		Utils::fixPath(path);
		mFile = fopen(path,"a");
		if( !mFile ){
			Trace::traceEvent(TRACE_ERROR, "Unable to open log file on %s : please specify a different directory (-d)", path);
			return ;
		}
		Trace::traceEvent(TRACE_NORMAL, "set Rolling File  to %s ",path);
		mLast += CONST_MAX_ACTIVITY_DURATION;
		localtime_r(&mLast,&l);
		Trace::traceEvent(TRACE_NORMAL, "next time is %d-%02d-%02d %02d:%02d:%02d ",l.tm_year+1900,l.tm_mon+1,l.tm_mday,l.tm_hour,l.tm_min,l.tm_sec);
	}
}
int CRollingFile::open(const char *fmt)
{
	char path[MAX_PATH];
	struct tm l;

	mNameFormat=fmt;
	mLast = time(NULL);
	localtime_r(&mLast,&l);
	l.tm_hour=l.tm_min=l.tm_sec=0;
	mLast = mktime(&l);
//	mLast -= (mLast % CONST_MAX_ACTIVITY_DURATION);
	
	snprintf(path, sizeof(path), mNameFormat, gPrefs->get_data_dir(),l.tm_year+1900,l.tm_mon+1,l.tm_mday,l.tm_hour);
	Utils::fixPath(path);
	mFile = fopen(path,"a");
	if( !mFile ){
		Trace::traceEvent(TRACE_ERROR, "Unable to open log file on %s : please specify a different directory (-d)", path);
		return FALSE;
	}
	Trace::traceEvent(TRACE_NORMAL, "set Rolling File  to %s ",path);
	mLast += CONST_MAX_ACTIVITY_DURATION;
	localtime_r(&mLast,&l);
	Trace::traceEvent(TRACE_NORMAL, "next time is %d-%02d-%02d %02d:%02d:%02d ",l.tm_year+1900,l.tm_mon+1,l.tm_mday,l.tm_hour,l.tm_min,l.tm_sec);
	return TRUE;
}

