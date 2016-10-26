
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vDefine.h"
#include "vCommon.h"
#include "vLog.h"
#include "vNet.h"
#include "vData.h"

#define vtag(y, x, ...)     Log->print(   Log, __func__, __LINE__,           y    , x, ##__VA_ARGS__)
#define vOK(x, ...)         Log->print(   Log, __func__, __LINE__, CCG"    OK"CCe , x, ##__VA_ARGS__)
#define vXX(x, ...)         Log->print(   Log, __func__, __LINE__, CCR"  Fail"CCe , x, ##__VA_ARGS__)
#define vC(x, ...)          Log->print(   Log, __func__, __LINE__, CCr"Client"CCe , x, ##__VA_ARGS__)
#define vS(x, ...)          Log->print(   Log, __func__, __LINE__, CCg"Server"CCe , x, ##__VA_ARGS__)

extern VLog* Log;

void sample_udpClient ()
{
	int skt;
	VNet* Client;
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
	VAddr vaddr;
	VNet* Server;
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
	VNet* Client;
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
	VAddr vaddr;
	VNet* Server;
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
	VTimer* Timer     = vtimer_create ();
	VLog*   LogString = vlog_create (3, "txt", "./log/", "YMD h:m:s.u | F10():L4 | S | V",  "YMD h:m:s.u | F10():L4 | S | V");
	int i;
	char buf[128];

	Timer->resume(Timer);
	LogString->print(LogString, __func__, __LINE__, "-----", "%s\n", Timer->nowString(buf, sizeof(buf), "YMD h:m:s.u"));
	for (i=0; i<1000; i++)
	{
		//vc_msleep (1000);
		memset (buf, 65+(i%26), sizeof(buf));
		buf[sizeof(buf)-1] = '\0';
		LogString->print(LogString, __func__, __LINE__, "     ", "%s\n", buf);
	}
	LogString->print(LogString, __func__, __LINE__, "-----", "%llu\n", Timer->diffms(Timer));
	vtimer_destroy (Timer);
}







