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
 * 1, Log File Size
 *
 * FIXME:
 *
 * 1. naming.
 *
 */


/*************************************************************************************/
/*************************************************************************************/
 
VLog* Log;

#define vlog(x, ...)      Log->print(   Log, __func__, __LINE__,    "      "    , x, ##__VA_ARGS__)
#define vtag(y, x, ...)   Log->print(   Log, __func__, __LINE__,           y    , x, ##__VA_ARGS__)

void init (void)
{
	vutillog_init (3, "./log/", "vutil.log");
	Log = vlog_create (  3,     "txt", "./log/", "h:m:s | F15():L4 | S6 | V",  "h:m:s | F15():L4 | S6 | V");
	vtag ("MEM", "mem=%llu\n", vc_getMemUsage());
}

void deinit (void)
{
	vtag ("MEM", "mem=%llu\n", vc_getMemUsage());
	vutillog_deinit ();
	vlog_destroy (Log);
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


void sample_task ()
{
	VTaskMng* TaskMng = vtaskmng_create ();
	//vmsleep (2000);
	//TaskMng->join (TaskMng);
	TaskMng->detach (TaskMng);
	vtaskmng_destroy (TaskMng);
	return ;
}







/*************************************************************************************/
/*************************************************************************************/

int main (int argc, char* argv[])
{

	init ();

#if 1
	sample_task ();
#endif
#if 0
	sample_datatree ();
#endif
#if 0
	sample_datalist ();
#endif
#if 0
	sample_log ();
#endif
#if 0
	sample_udpServer ();
#endif
#if 0
	sample_udpClient ();
#endif
#if 0
	sample_tcpClient ();
#endif
#if 0
	sample_tcpServer ();
#endif

	deinit ();

	return 0;
}

/*************************************************************************************/
/*************************************************************************************/




