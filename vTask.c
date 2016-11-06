
#include "vTask.h"

static void* vtask_routin (void* arg);




/*************************************************************************************/
/*************************************************************************************/

u64t vtask_newId (void)
{
	static u64t id=0;
	return  __sync_add_and_fetch (&id, 1);
}

/*************************************************************************************/
/*************************************************************************************/

VTaskMng* vtaskmng_create ()
{
	VTaskMng* pTaskMng = vc_malloc (sizeof(VTaskMng));
	if (!pTaskMng)
	{
		return NULL;
	}

	vzero (pTaskMng, sizeof(VTaskMng));
	pTaskMng->join    = vtaskmng_join;
	pTaskMng->detach  = vtaskmng_detach;

	pthread_mutex_init (&pTaskMng->mutex, NULL);
	pthread_create (&pTaskMng->tid, NULL, vtask_routin, pTaskMng);
	vp ("pTaskMng->tid=%lu\n", pTaskMng->tid);

	return pTaskMng;
}

void vtaskmng_destroy (VTaskMng* pTaskMng)
{
	vc_free (pTaskMng, sizeof(VTaskMng));
}

void vtaskmng_join (void* self)
{
	VTaskMng* pTaskMng = (VTaskMng*) self;
	int status = __sync_fetch_and_add(&pTaskMng->eJoinDetach, JOIN);
	if (status==0)
	{
		pthread_join (pTaskMng->tid, NULL);
	}
	return ;
}

void vtaskmng_detach (void* self)
{
	VTaskMng* pTaskMng = (VTaskMng*) self;
	int status = __sync_fetch_and_add(&pTaskMng->eJoinDetach, DETACH);
	if (status==0)
	{
		pthread_detach (pTaskMng->tid);
	}
	return ;
}


/*************************************************************************************/
/*************************************************************************************/

static void* vtask_routin (void* arg)
{
	VTaskMng* pTaskMng = (VTaskMng*) arg;
	vmsleep (500);
	vp ("pthread_self()=%lu\n", pthread_self());
	vp ("pTaskMng->tid=%lu\n", pTaskMng->tid);
	return NULL;
}





