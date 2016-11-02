
#ifndef __VTASK_H__
#define __VTASK_H__

#include <pthread.h>

#include "vDefine.h"
#include "vCommon.h"
#include "vData.h"
#include "vLog.h"

typedef void*(*routine_ft)(void* arg);

typedef enum {
	JOIN =1,
	DETACH =2,
} TaskStatus_e;

typedef struct {
	int         id;
	int         interval;
	int         lifecycle;
	void*       (*routine)(void* arg);
	void*       routine_arg;
} VTask;

typedef struct {
	/* method. */
	void    (*join)   (void* self);
	void    (*detach) (void* self);

	/* data. */
	TaskStatus_e  eJoinDetach;
	vthread_t     tid;
	VDataList*    Queue;
	vmutex_t      mutex;
} VTaskMng;


VTaskMng* vtaskmng_create   ();
void   vtaskmng_destroy  (VTaskMng* pTaskMng);
void   vtaskmng_join     (void* self);
void   vtaskmng_detach   (void* self);



void vtask_add (void* self, int interval, int lifecycle, routine_ft todo, void* arg);

#endif  /* __VTASK_H__ */

//VTaskMng* pTaskMng = (VTaskMng*) self;

