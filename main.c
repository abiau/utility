#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vDefine.h"
#include "vCommon.h"
#include "vLog.h"
#include "vNet.h"
#include "vData.h"
#include "vTask.h"

/*************************************************************************************/
/*************************************************************************************/
/*
 * TODO:
 *
 * FIXME:
 *
 * 1. naming.
 *
 */
/*************************************************************************************/
/*************************************************************************************/
 
VLog* MyLog;


void init (void)
{
	printf ("vmemory=%llu\n", vc_getMemUsage());
	vlog_init (3, "./log/", "vutil.log");
	MyLog = vlog_create (  3, "./log/","txt", "h:m:s | F15():L4 | S6 | V",  "h:m:s | F15():L4 | S6 | V");
	//vtag ("MEM", "mem=%llu\n", vc_getMemUsage());
}

void deinit (void)
{
	//vtag ("MEM", "mem=%llu\n", vc_getMemUsage());
	vlog_destroy (MyLog);
	vlog_deinit ();
	printf ("vmemory=%llu\n", vc_getMemUsage());
}

/*************************************************************************************/
/*************************************************************************************/

extern void sample_log ();
extern void sample_udpServer ();
extern void	sample_udpClient ();
extern void sample_tcpServer ();
extern void	sample_tcpClient ();
extern void sample_datalist  ();
extern void sample_datatree  ();
extern void sample_task ();

/*************************************************************************************/
/*************************************************************************************/

void* task (void* arg)
{
	size_t n = (size_t)arg;
	vlog ("%zd\n", n);
	return NULL;
}

void sample_task ()
{
#if 0
	vtag ("MEM", "mem=%llu\n", vc_getMemUsage());
	VTPool* TPoll = vtpool_create (1, 10);
	vtpool_destroy (TPoll);
#endif

	size_t   i;
	VTask*   pTask;
	VThread* Thread = vthread_create();
	
	vmsleep (200);
	
	Thread->run (Thread);
	for (i=0; i<5; i++)
	{
		pTask = vtask_create (task, (void*)i, 0, 0);
		Thread->addTask(Thread, pTask);
		vmsleep (400);
	}

	Thread->ready (Thread);
	Thread->exit (Thread);
	Thread->clearTask(Thread);
	Thread->join (Thread);
	vthread_destory (Thread);
	return ;
}




/*************************************************************************************/
/*************************************************************************************/

int main (int argc, char* argv[])
{
	init ();

#if 1
	sample_task ();
#else
	sample_datatree ();
	sample_datalist ();
	sample_log ();
	sample_udpServer ();
	sample_udpClient ();
	sample_tcpClient ();
	sample_tcpServer ();
#endif

	deinit ();

	return 0;
}

/*************************************************************************************/
/*************************************************************************************/




