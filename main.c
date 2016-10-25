#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vDefine.h"
#include "vCommon.h"
#include "vLog.h"
#include "vNet.h"
#include "vData.h"


#define vlog(x, ...)        Log->print(   Log, __func__, __LINE__,    "      "    , x, ##__VA_ARGS__)
#define vtag(y, x, ...)     Log->print(   Log, __func__, __LINE__,           y    , x, ##__VA_ARGS__)
#define vOK(x, ...)         Log->print(   Log, __func__, __LINE__, CCG"    OK"CCe , x, ##__VA_ARGS__)
#define vXX(x, ...)         Log->print(   Log, __func__, __LINE__, CCR"  Fail"CCe , x, ##__VA_ARGS__)
#define vC(x, ...)          Log->print(   Log, __func__, __LINE__, CCr"Client"CCe , x, ##__VA_ARGS__)
#define vS(x, ...)          Log->print(   Log, __func__, __LINE__, CCg"Server"CCe , x, ##__VA_ARGS__)

static VLog* Log;

void init (void)
{
	verr_init (3, "err.log", "./log/");
	Log = vlog_create (  3,     "txt", "./log/", "YMD h:m:s.u | F20():L4 | S | V",  "YMD h:m:s.u | F20():L4 | S | V");
	vlog ("[MEM] mem=%llu\n", vc_getMemUsage());
}

void deinit (void)
{
	vlog ("[MEM] mem=%llu\n", vc_getMemUsage());
	verr_deinit ();
	vlog_destroy (Log);
}

/*************************************************************************************/
/*************************************************************************************/

static int _SmallerThan (void* itemInQueue, void* arg);

static int _equal (void* itemInQueue, void* arg)
{
	if (*(int*)itemInQueue == *(int*)arg)
	{
		return OK;
	}
	else
	{
		return FAIL;
	}
}

static int _do_print (void* arg)
{
	vlog ("[TRAVEL]  arg=%d\n", *(int*)arg);
	return 0;
}

static int _SmallerThan (void* itemInQueue, void* arg)
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

void sample_datatree ()
{
	VDataNode* p;
	VDataTree* Tree = vdatatree_create ();
#if 1
	p = Tree->insert (Tree, _SmallerThan, int_new(8));
	p = Tree->insert (Tree, _SmallerThan, int_new(4));
	p = Tree->insert (Tree, _SmallerThan, int_new(2));
	p = Tree->insert (Tree, _SmallerThan, int_new(6));
	p = Tree->insert (Tree, _SmallerThan, int_new(12));
	p = Tree->insert (Tree, _SmallerThan, int_new(10));
	p = Tree->insert (Tree, _SmallerThan, int_new(14));
#else
	p = Tree->insert (Tree, _SmallerThan, int_new(4));
	p = Tree->insert (Tree, _SmallerThan, int_new(2));
	p = Tree->insert (Tree, _SmallerThan, int_new(6));
#endif
	vdatatree_Travel (Tree, Tree->head, _do_print);
	p = vdatatree_TravelFind (Tree, Tree->head, _SmallerThan, int_new(6));
	if (p) {
		vlog ("[FIND] id=[%2d], arg=%d\n", p->id, *(int*)p->arg);
	} else {
		vlog ("[FIND] none.\n");
	}
	//vdatatree_Delete (Tree, p, (destructor_ft)int_del);
	vdatatree_destroy (Tree);
}

#if 0
void PP (VDataList* pList) 
{
	/* Foreach & Delete */
	pList->seek (pList, pList->head);
	while(1)
	{
		VDataNode* p = pList->foreach (pList, HEAD, NULL, int_new(10));
		if (!p)
		{
			break;
		}
		vlog ("[FOREACH] id=%2d, arg=%d\n", p->id, *(int*)p->arg);
		//pList->delete (pList, p, (destructor_ft)int_del);
	}
	vlog ("\n\n");
	
}
#endif


void sample_datalist ()
{
	int* pItem;
	VDataNode* p;
	VDataList* List = vdatalist_create ();
	
	/* Insert */
	p = List->insert (List, HEAD, _SmallerThan, int_new(3));
	p = List->insert (List, HEAD, _SmallerThan, int_new(2));
	p = List->insert (List, HEAD, _SmallerThan, int_new(9));
	p = List->insert (List, HEAD, _SmallerThan, int_new(6));
	p = List->insert (List, HEAD, _SmallerThan, int_new(1));
	p = List->insert (List, HEAD, _SmallerThan, int_new(4));
	vlog ("[INFO] size=%d\n", List->size);

	/* Search */
	p = List->search (List, HEAD, _equal, pItem=int_new(2));
	if (p)
	{
		vlog ("[SEARCH] id=%2d, arg=%d\n", p->id, *(int*)p->arg);
	}
	else
	{
		vlog ("[SEARCH] none.\n");
	}
	int_del(pItem);

#if 1
	/* Foreach & Delete */
	List->seek (List, List->head);
	while(1)
	{
		p = List->foreach (List, HEAD, _SmallerThan, int_new(10));
		if (!p)
		{
			break;
		}
		vlog ("[FOREACH] id=%2d, arg=%d\n", p->id, *(int*)p->arg);
		List->delete (List, p, (destructor_ft)int_del);
	}
#endif

	vdatalist_destroy (List);
	
	return ;
}

void sample_data ()
{
	return ;
}





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


/*************************************************************************************/
/*************************************************************************************/



int main (int argc, char* argv[])
{

	init ();

#if 1
	sample_datatree ();
#else
	sample_datalist ();
#endif
#if 0
	sample_data ();
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




