#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vDefine.h"
#include "vCommon.h"
#include "vLog.h"
#include "vNet.h"


#define vtag(y, x, ...)     Tag->print(   Tag, __func__, __LINE__,              y, x, ##__VA_ARGS__)
#define vOK(x, ...)         Tag->print(   Tag, __func__, __LINE__, CCG"    OK"CCe, x, ##__VA_ARGS__)
#define vXX(x, ...)         Tag->print(   Tag, __func__, __LINE__, CCR"  Fail"CCe, x, ##__VA_ARGS__)
#define vC(x, ...)          Tag->print(   Tag, __func__, __LINE__, CCr"Client"CCe, x, ##__VA_ARGS__)
#define vS(x, ...)          Tag->print(   Tag, __func__, __LINE__, CCg"Server"CCe, x, ##__VA_ARGS__)

VLog_st*    Tag;

void init (void)
{
	verr_init (3, "err.log", "./log/");
	Tag   = vlog_create (  3,     "txt", "./log/", "YMD h:m:s.u | F10():L4 | S | V",  "YMD h:m:s.u | F10():L4 | S | V");
}

void deinit (void)
{
	verr_deinit ();
	vlog_destroy (Tag);
}


/*************************************************************************************/
/*************************************************************************************/

void sample_udpClient ()
{
	int skt;
	VNet_st* Client;
	char  buf[1024];
	char* str = "Hello\n";

	memset (buf, 0, sizeof(buf));
	memcpy (buf, str, strlen(str)+1);

	Client = vnet_create ('u', 5566);
	skt = Client->getskt (Client);
	Client->connect (Client, "127.0.0.1", 5566);
	vC ("skt=%d\n", skt);
	while (1)
	{
		sleep (1);
		Client->write (Client, skt, buf, sizeof(buf));
		vC (buf);
	}
	vnet_destroy (Client);
}

void sample_udpServer ()
{
	int skt;
	VAddr_st vaddr;
	VNet_st* Server;
	char  buf[1024];

	Server = vnet_create ('u', 5566);
	skt = Server->getskt (Server);
	vS ("skt=%d\n", skt);
	while (1)
	{
		memset (buf, 0, sizeof(buf));
		Server->recvfrom (Server, skt, buf, sizeof(buf), &vaddr);
		vS ("ip:port=[%s:%d]  %s\n", vaddr.ip, vaddr.port, buf);
	}
	vnet_destroy (Server);
}

void sample_tcpClient ()
{
	int skt;
	VNet_st* Client;
	char  buf[1024];
	char* str = "Hello";

	memset (buf, 0, sizeof(buf));
	memcpy (buf, str, strlen(str)+1);

	Client = vnet_create ('t', 5566);
	skt = Client->getskt (Client);
	Client->connect (Client, "127.0.0.1", 5566);
	vC ("skt=%d\n", skt);
	while (1)
	{
		sleep (1);
		Client->write (Client, skt, buf, sizeof(buf));
		vC ("%s\n", buf);
	}
	vnet_destroy (Client);
}

void sample_tcpServer ()
{
	int ret;
	int skt;
	int sktSession;
	VAddr_st vaddr;
	VNet_st* Server;
	char  buf[1024];

	Server = vnet_create ('t', 5566);
	skt = Server->getskt (Server);
	Server->listen (Server, 10);
	vS ("skt=%d\n", skt);
	while (1)
	{
		sktSession = Server->accept (Server, &vaddr);
		vS ("sktSession=%d, ip:port=[%s:%d]\n", sktSession, vaddr.ip, vaddr.port);
		while (1)
		{
			memset (buf, 0, sizeof(buf));
			ret = Server->read (Server, sktSession, buf, sizeof(buf));
			if (ret<=0) { break; }
			vS ("%s\n", buf);
		}
		_vnet_Close (sktSession);
	}
	vnet_destroy (Server);
}


void sample_log ()
{
	VTimer_st*  Timer;
	Timer = vtimer_create ();
	VLog_st* Log = vlog_create (3, "txt", "./log/", "YMD h:m:s.u | F10():L4 | S | V",  "YMD h:m:s.u | F10():L4 | S | V");
	int i;
	char buf[128];

	Timer->resume(Timer);
	Log->print(Log, __func__, __LINE__, "-----", "%s\n", Timer->nowString(buf, sizeof(buf), "YMD h:m:s.u"));
	for (i=0; i<1000; i++)
	{
		//vc_msleep (1000);
		memset (buf, 65+(i%26), sizeof(buf));
		buf[sizeof(buf)-1] = '\0';
		Log->print(Log, __func__, __LINE__, "     ", "%s\n", buf);
	}
	Log->print(Log, __func__, __LINE__, "-----", "%llu\n", Timer->diffms(Timer));
	vtimer_destroy (Timer);
}


/*************************************************************************************/
/*************************************************************************************/



int main (int argc, char* argv[])
{

	init ();

	
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




