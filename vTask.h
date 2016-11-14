
#ifndef __VTASK_H__
#define __VTASK_H__

#include <pthread.h>

#include "vDefine.h"
#include "vCommon.h"
#include "vData.h"
#include "vLog.h"

typedef void*(*task_ft)(void* arg);

typedef struct {
	int         id;
	int         interval;
	int         lifecycle;
	void*       (*func)(void* arg);
	void*       arg;
} VTask;

VTask* vtask_create   (task_ft func, void* arg, int interval, int lifecycle);
void   vtask_destory  (VTask* pTask);

typedef enum {
	THR_READY   = 1,
	THR_RUN     = 2,
	THR_EXIT    = 3,
	THR_JOIN    = 101,
	THR_DETACH  = 102,
} ThreadStatus_e;

typedef struct {
	void    (*join)        (void* self);
	void    (*detach)      (void* self);
	void    (*ready)       (void* self);
	void    (*run)         (void* self);
	void    (*exit)        (void* self);
	void    (*addTask)     (void* self, VTask* pTask);
	void    (*delTask)     (void* self, VTask* pTask);
	void    (*clearTask)   (void* self);
	VList*          TaskQ;
	vthread_t       tid;
	ThreadStatus_e  Join_or_Detach;
	ThreadStatus_e  status;
} VThread;

VThread* vthread_create     ();
void     vthread_destory    (VThread* pThread);
void     vthread_join       (void* self);
void     vthread_detach     (void* self);
void     vthread_ready      (void* self);
void     vthread_run        (void* self);
void     vthread_exit       (void* self);
void     vthread_addTask    (void* self, VTask* pTask);
void     vthread_delTask    (void* self, VTask* pTask);
void     vthread_clearTask  (void* self);

typedef struct {
	/* method. */

	/* data. */
	VList*    ThreadQ;
	VList*    TaskQ;
	int       MaxThreads;
	int       MaxQueues;
} VTPool;

VTPool*  vtpool_create    (int MaxThreads, int nMaxQueues);
void     vtpool_destroy   (VTPool* pTPool);
void     vtpool_addTask   (void* self, task_ft func, void* arg);
void     vtpool_delTask   (void* self);





#endif  /* __VTASK_H__ */

//VThread* pThread = self;
//VTPool*  pTPool = self;

