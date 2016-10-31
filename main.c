#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vDefine.h"
#include "vCommon.h"
#include "vLog.h"
#include "vNet.h"
#include "vData.h"

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

#define vlog(x, ...)        Log->print(   Log, __func__, __LINE__,    "      "    , x, ##__VA_ARGS__)
#define vtag(y, x, ...)     Log->print(   Log, __func__, __LINE__,           y    , x, ##__VA_ARGS__)

void init (void)
{
	verr_init (3, "./log/", "err.log");
	Log = vlog_create (  3,     "txt", "./log/", "M/D h:m:s | F20():L4 | S8 | V",  "M/D h:m:s | F20():L4 | S8 | V");
	vtag ("MEM", "mem=%llu\n", vc_getMemUsage());
}

void deinit (void)
{
	vtag ("MEM", "mem=%llu\n", vc_getMemUsage());
	verr_deinit ();
	vlog_destroy (Log);
}

/*************************************************************************************/
/*************************************************************************************/


/*************************************************************************************/
/*************************************************************************************/

extern void sample_log ();
extern void sample_udpServer ();
extern void	sample_udpClient ();
extern void sample_tcpServer ();
extern void	sample_tcpClient ();
extern void sample_datalist  ();
extern void sample_datatree  ();

int main (int argc, char* argv[])
{

	init ();

#if 0
	sample_datatree ();
#endif
#if 0
	sample_datalist ();
#endif
#if 1
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




