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
#include <limits.h> 

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

typedef enum {
	PATH = 1,
	SIZE = 2,
	ROTATE = 3,
} SetVLogType_e;

/* VLOG */
typedef struct {
	/* method. */
	void    (*print)     (void* self, const char* szFunc, int nLine, ...);
	void    (*set)       (void* self, SetVLogType_e type, ...);
    void    (*setPath)   (void* self, char* folder, char* file);
    void    (*setSize)   (void* self, char* str);
    void    (*setRotate) (void* self, char* str);

	/* data. */
	vmutex_t         mutex;
	int              mode;
	n64t             nFileSize;
	n64t             MaxFileSize;
	n64t             lastTs;
	n64t             MaxRotateMs;
	char             folder   [LEN128];
	char             path     [LEN256];
	char             FmtScreen[LEN64];
	char             FmtFile  [LEN64];
	char             buf      [LEN2048];
} VLog;

extern VLog*  LOG_ERRNO;

VLog* vlog_create       (int mode, char* folder, char* file, char* FmtScreen, char* FmtFile);
void  vlog_destroy      (VLog* pLog);
void  vlog_print        (void* self, const char* szFunc, int nLine, ...);
void  vlog_set          (void* self, SetVLogType_e type, ...);
void  vlog_setPath      (void* self, char* folder, char* file);
void  vlog_setSize      (void* self, char* str);
void  vlog_setRotate    (void* self, char* str);


/* VTIMER */
typedef struct {
	/* method. */
	void             (*resume)    (void* self);
	u64t             (*diffms)    (void* self);
	u64t             (*diffus)    (void* self);
	u64t             (*now)       (void);
	char*            (*nowStr)    (char* buf, int bufLen, char* fmt);
	char*            (*tsStr)     (char* buf, int bufLen, char* fmt, u64t ms);
	/* data. */
	pthread_mutex_t  mutex;
	struct timeval   tv;
} VTimer;

VTimer* vtimer_create  ();
void    vtimer_destroy (VTimer* pTimer);


typedef struct {
	u64t ts;
	int  Y;
	int  M;
	int  D;
	int  h;
	int  m;
	int  s;
} DateTime;



#endif  /* __VLOG_H__ */


