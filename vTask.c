
#include "vTask.h"

static void* vthread_routin (void* arg);




/*************************************************************************************/
/*************************************************************************************/

u64t vtask_newId (void)
{
	static u64t id=0;
	return  __sync_add_and_fetch (&id, 1);
}

VTask* vtask_create (task_ft func, void* arg, int interval, int lifecycle)
{
	VTask* pTask = vc_malloc (sizeof(VTask)) VACCERT(pTask);

	pTask->func = func;
	pTask->arg = arg;
	pTask->interval = interval;
	pTask->lifecycle = lifecycle;

	return pTask;
}

void vtask_destory (VTask* pTask)
{
	vc_free (pTask, sizeof(VTask));
}

/*************************************************************************************/
/*************************************************************************************/

VTPool* vtpool_create (int MaxThreads, int MaxQueues)
{
	VTPool* pTPool = vc_malloc (sizeof(VTPool)) VACCERT(pTPool);

	pTPool->ThreadQ = vlist_create ();
	pTPool->TaskQ   = vlist_create ();
	
	pTPool->MaxThreads = MaxThreads;
	pTPool->MaxQueues  = MaxQueues;

	return pTPool;
}

void vtpool_destroy (VTPool* pTPool)
{
	vlist_destroy (pTPool->ThreadQ);
	vlist_destroy (pTPool->TaskQ);
	vc_free (pTPool, sizeof(VTPool));
}

/*************************************************************************************/
/*************************************************************************************/

void vtpool_addTask (void* self, task_ft func, void* arg)
{
	VTPool*  pTPool = self;
}

void vtpool_delTask (void* self)
{
	VTPool*  pTPool = self;
}




/*************************************************************************************/
/*************************************************************************************/

VThread* vthread_create ()
{
	VThread* pThread   = vc_malloc (sizeof(VThread)) VACCERT(pThread);
	pThread->join      = vthread_join;
	pThread->detach    = vthread_detach;
	pThread->ready     = vthread_ready;
	pThread->run       = vthread_run;
	pThread->exit      = vthread_exit;
	pThread->addTask   = vthread_addTask;
	pThread->delTask   = vthread_delTask;
	pThread->clearTask = vthread_clearTask;

	pThread->TaskQ   = vlist_create();
	pThread->status  = THR_READY;
	pthread_create (&pThread->tid, NULL, vthread_routin, pThread);
	return pThread;
}

void vthread_destory (VThread* pThread)
{
	vlist_destroy (pThread->TaskQ);
	vc_free (pThread, sizeof(VThread));
}

void vthread_join (void* self)
{
	VThread* pThread = self;
	int status = __sync_fetch_and_add(&pThread->Join_or_Detach, THR_JOIN);
	if (status==0)
	{
		pthread_join (pThread->tid, NULL);
	}
	return ;
}

void vthread_detach (void* self)
{
	VThread* pThread = self;
	int status = __sync_fetch_and_add(&pThread->Join_or_Detach, THR_DETACH);
	if (status==0)
	{
		pthread_detach (pThread->tid);
	}
	return ;
}

void vthread_ready (void* self)
{
	VThread* pThread = self;
	pThread->status = THR_READY;
	return ;
}

void vthread_run (void* self)
{
	VThread* pThread = self;
	pThread->status = THR_RUN;
	return ;
}

void vthread_exit (void* self)
{
	VThread* pThread = self;
	pThread->status = THR_EXIT;
	return ;
}

#if 0
static int _lessThan (void* itemInQueue, void* arg)
{
	if (*(int*)itemInQueue < *(int*)arg)
	{
		return OK;
	}
	else
	{
		return FAIL;
	}
}
#endif

void vthread_addTask (void* self, VTask* pTask)
{
	VThread* pThread = self;
	VList* TaskQ = pThread->TaskQ;
	TaskQ->insert (TaskQ, NULL, pTask);
	return ;
}

static int _compare_TaskId (void* item, void* arg)
{
	VTask* pItem = item;
	VTask* pArg = arg;
	return (pItem->id == pArg->id) ? OK : FAIL;
}

void vthread_delTask (void* self, VTask* pTask)
{
	VThread* pThread = self;
	VList* TaskQ = pThread->TaskQ;
	VNode* pNode = TaskQ->search (TaskQ, _compare_TaskId, pTask);
	TaskQ->delete (TaskQ, pNode, NULL);
	return ;
#if 0
	TaskQ->seek (TaskQ);
	VNode* pNode = TaskQ->foreach (TaskQ, NULL, NULL);
	VTask* pTask = pNode->arg;
	TaskQ->delete (TaskQ, pNode, NULL);
	return pTask;
#endif
}

void vthread_clearTask (void* self)
{
	VThread* pThread = self;
	VList* TaskQ = pThread->TaskQ;
	VNode* pNode;
	TaskQ->seek(TaskQ);
	while ((pNode=(TaskQ->foreach(TaskQ, NULL, NULL))))
	{
		VTask* pTask = pNode->arg;
		vtask_destory (pTask);
		TaskQ->delete (TaskQ, pNode, NULL);
	}
}

/*************************************************************************************/
/*************************************************************************************/

static void* vthread_routin (void* arg)
{
	VThread* pThread = arg;
	VList* TaskQ = pThread->TaskQ;

	while (1)
	{
		switch (pThread->status)
		{
		case THR_READY:
			{
				vp ("THR_READY\n");
				vmsleep (1000);
			}
			break;
		case THR_RUN:
			{
#if 0
				vmsleep (1);
#else
				vp ("THR_RUN\n");
				vmsleep (1000);
#endif
				VNode* pNode=NULL;
				TaskQ->seek(TaskQ);
				while ((pNode=(TaskQ->foreach(TaskQ, NULL, NULL))))
				{
					VTask* pTask=pNode->arg;
					pTask->func(pTask->arg);
				}
			}
			break;
		case THR_EXIT:
		default:
			{
				vp ("THR_EXIT\n");
				pthread_exit (NULL);
			}
			break;
		} /* switch */
	} /* while */

	return NULL;
}





