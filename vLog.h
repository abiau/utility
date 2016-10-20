#ifndef __VLOG_H__
#define __VLOG_H__

#include <errno.h>
#include "vDefine.h"
#include "vCommon.h"



#define verr(y) do{ \
	if(ERR) { ERR->print(ERR, __func__, __LINE__, "%s(), errno=%d|%s\n", y, errno, strerror(errno)); \
	}; }while(0)

#define verr_init(a,b,c) do{ \
	if(ERR==NULL) { \
		ERR = vlog_create ((a),(b),(c),"YMD h:m:s.u | F10():L4 | V","YMD h:m:s.u | F10():L4 | V"); \
	}; }while(0)

#define verr_deinit() do{ \
	if(ERR!=NULL) { \
		vlog_destroy (ERR); \
	}; }while(0)

/***************************************************************************/
/***************************************************************************/

/* VLOG */
typedef struct {
	/* method. */
	void             (*print) (void* self, const char* szFunc, int nLine, ...);
	/* data. */
	pthread_mutex_t  m_mutex;
	int              m_mode;
	char             m_path     [LEN256];
	char             m_fmtScreen[LEN64];
	char             m_fmtFile  [LEN64];
	char             m_buf      [LEN2048];
} VLog_st;

extern VLog_st*  ERR;

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


