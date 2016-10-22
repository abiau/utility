#ifndef __VLOG_H__
#define __VLOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <errno.h>

#include "vDefine.h"
#include "vCommon.h"



#define verr(y) do{ \
	if(LOG_ERRNO) { LOG_ERRNO->print(LOG_ERRNO, __func__, __LINE__, "%s(), errno=%d|%s\n", y, errno, strerror(errno)); \
	}; }while(0)

#define verr_init(a,b,c) do{ \
	if(LOG_ERRNO==NULL) { \
		LOG_ERRNO = vlog_create ((a),(b),(c),"YMD h:m:s.u | F10():L4 | V","YMD h:m:s.u | F10():L4 | V"); \
	}; }while(0)

#define verr_deinit() do{ \
	if(LOG_ERRNO!=NULL) { \
		vlog_destroy (LOG_ERRNO); \
	}; }while(0)

/***************************************************************************/
/***************************************************************************/

/* VLOG */
typedef struct {
	/* method. */
	void             (*print) (void* self, const char* szFunc, int nLine, ...);

	/* data. */
	vmutex_t         m_mutex;
	int              m_mode;
	char             m_path     [LEN256];
	char             m_fmtScreen[LEN64];
	char             m_fmtFile  [LEN64];
	char             m_buf      [LEN2048];
} VLog_st;

extern VLog_st*  LOG_ERRNO;

VLog_st*   vlog_create    (int mode, char* file, char* folder, char* fmtScreen, char* fmtFile);
void       vlog_destroy   (VLog_st* pLog);


/* VTIMER */
typedef struct {
	/* method. */
	void             (*resume)    (void* self);
	u64t             (*diffms)    (void* self);
	u64t             (*diffus)    (void* self);
	u64t             (*now)       (void);
	char*            (*nowString) (char* buf, int bufLen, char* fmt);
	char*            (*tsString)  (char* buf, int bufLen, char* fmt, u64t ms);
	/* data. */
	pthread_mutex_t  m_mutex;
	struct timeval   m_tv;
} VTimer_st;

VTimer_st* vtimer_create  ();
void       vtimer_destroy (VTimer_st* pTimer);


typedef struct {
	u64t ts;
	int  yy;
	int  mm;
	int  dd;
	int  h;
	int  m;
	int  s;
} DateTime_st;



#endif  /* __VLOG_H__ */


