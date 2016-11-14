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
#include "vLock.h"


/***************************************************************************/
/***************************************************************************/

#define vlog_init(a,b,c) do{ \
	if(VUTILLOG==NULL) { \
		VUTILLOG = vlog_create ((a),(b),(c),"h:m:s | F15():L4 | S6 | V","h:m:s | F15():L4 | S6 | V"); \
	}; }while(0)

#define vlog_deinit() do{ \
	if(VUTILLOG!=NULL) { \
		vlog_destroy (VUTILLOG); \
	}; }while(0)

#define vp(x, ...) do{ \
	if(VUTILLOG) { VUTILLOG->print(VUTILLOG, __func__, __LINE__, "",   x, ##__VA_ARGS__); \
	}; }while(0)
#define vlog vp

#define vt(y, x, ...) do{ \
	if(VUTILLOG) { VUTILLOG->print(VUTILLOG, __func__, __LINE__,  y,   x, ##__VA_ARGS__); \
	}; }while(0)
#define vtag vt

#define ve(e) do{ \
	if(VUTILLOG) { VUTILLOG->print(VUTILLOG, __func__, __LINE__, "",   "%s(), errno=%d|%s\n", e, errno, strerror(errno)); \
	}; }while(0)
#define verr ve

/***************************************************************************/
/***************************************************************************/

#define VLOG_BASE \
	struct {\
		void    (*print)     (void* self, const char* szFunc, int nLine, ...);\
		void    (*set)       (void* self, SetVLogType_e type, ...);\
		void    (*setPath)   (void* self, char* folder, char* file);\
		void    (*setSize)   (void* self, char* GMKB);\
		void    (*setRotate) (void* self, char* UYMWDhms);\
	};\


typedef enum {
	PATH = 1,
	SIZE = 2,
	ROTATE = 3,
} SetVLogType_e;

/***************************************************************************/
/***************************************************************************/

/* VLOG */
typedef struct {
	/* method. */
	VLOCK_BASE;
	VLOG_BASE;

	/* data. */
	VLock*           Lock;
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

extern VLog*  VUTILLOG;

VLog* vlog_create       (int mode, char* folder, char* file, char* FmtScreen, char* FmtFile);
void  vlog_destroy      (VLog* pLog);
void  vlog_print        (void* self, const char* szFunc, int nLine, ...);
void  vlog_set          (void* self, SetVLogType_e type, ...);
void  vlog_setPath      (void* self, char* folder, char* file);
void  vlog_setSize      (void* self, char* GMKB);
void  vlog_setRotate    (void* self, char* UYMWDhms);
void  vlog_lock         (void* self);
void  vlog_unlock       (void* self);


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


