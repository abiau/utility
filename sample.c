
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vDefine.h"
#include "vCommon.h"
#include "vLog.h"
#include "vNet.h"
#include "vData.h"

#define mytag(y, x, ...)     MyLog->print( MyLog, __func__, __LINE__,           y    , x, ##__VA_ARGS__ )
#define myOK(x, ...)         MyLog->print( MyLog, __func__, __LINE__, CCG"    OK"CCe , x, ##__VA_ARGS__ )
#define myXX(x, ...)         MyLog->print( MyLog, __func__, __LINE__, CCR"  Fail"CCe , x, ##__VA_ARGS__ )
#define myC(x, ...)          MyLog->print( MyLog, __func__, __LINE__, CCr"Client"CCe , x, ##__VA_ARGS__ )
#define myS(x, ...)          MyLog->print( MyLog, __func__, __LINE__, CCg"Server"CCe , x, ##__VA_ARGS__ )

extern VLog* MyLog;

void sample_udpClient ()
{
	int   skt;
	char  buf[1024];
	char* str = "Hello\n";

	vzero (buf, buf);
	memcpy (buf, str, strlen(str)+1);

	VNet* Client = vnet_create (UDP, 5566);
	skt = Client->getskt (Client);
	Client->connect (Client, "127.0.0.1", 5566);
	myC ("skt=%d\n", skt);
	while (1)
	{
		vmsleep (1000);
		Client->write (Client, skt, buf, sizeof(buf));
		myC (buf);
	}
	vnet_destroy (Client);
}

void sample_udpServer ()
{
	int   skt;
	VAddr vaddr;
	char  buf[1024];

	VNet* Server = vnet_create (UDP, 5566);
	skt = Server->getskt (Server);
	myS ("skt=%d\n", skt);
	while (1)
	{
		vzero (buf, buf);
		Server->recvfrom (Server, skt, buf, sizeof(buf), &vaddr);
		myS ("ip:port=[%s:%d]  %s\n", vaddr.ip, vaddr.port, buf);
	}
	vnet_destroy (Server);
}

void sample_tcpClient ()
{
	int   skt;
	char  buf[1024];
	char* str = "Hello";

	vzero (buf, buf);
	memcpy (buf, str, strlen(str)+1);

	VNet* Client = vnet_create (TCP, 5566);
	skt = Client->getskt (Client);
	Client->connect (Client, "127.0.0.1", 5566);
	myC ("skt=%d\n", skt);
	while (1)
	{
		vmsleep (1000);
		Client->write (Client, skt, buf, sizeof(buf));
		myC ("%s\n", buf);
	}
	vnet_destroy (Client);
}

void sample_tcpServer ()
{
	int   ret;
	int   skt;
	int   sktSession;
	VAddr vaddr;
	char  buf[1024];

	VNet* Server = vnet_create (TCP, 5566);
	skt = Server->getskt (Server);
	Server->listen (Server, 10);
	myS ("skt=%d\n", skt);
	while (1)
	{
		sktSession = Server->accept (Server, &vaddr);
		myS ("sktSession=%d, ip:port=[%s:%d]\n", sktSession, vaddr.ip, vaddr.port);
		while (1)
		{
			vzero (buf, buf);
			ret = Server->read (Server, sktSession, buf, sizeof(buf));
			if (ret<=0) { break; }
			myS ("%s\n", buf);
		}
		vnet_Close (sktSession);
	}
	vnet_destroy (Server);
}


void sample_log ()
{
	VTimer* Timer     = vtimer_create ();
	VLog*   LogString = vlog_create (3, "./log/", "txt", "YMD h:m:s.u | F10():L4 | S | V",  "YMD h:m:s.u | F10():L4 | S | V");
	LogString->setPath   (LogString, "./log/", "txt");
	LogString->setSize   (LogString, "10M");
	LogString->setRotate (LogString, "1D");

	int  i;
	char timeString[32];
	char buf[32];

	Timer->resume(Timer);
	Timer->nowStr(timeString, sizeof(timeString), "YMD h:m:s.u");
	LogString->print(LogString, __func__, __LINE__, "-----", "%s\n", timeString);
	for (i=0; i<30; i++)
	{
		vmsleep (300);
		memset (buf, 65+(i%26), sizeof(buf));
		buf[sizeof(buf)-1] = '\0';
		LogString->print(LogString, __func__, __LINE__, "     ", "%s\n", buf);
	}
	LogString->print(LogString, __func__, __LINE__, "-----", "%llu\n", Timer->diffms(Timer));
	vtimer_destroy (Timer);
}




static int _equal (void* itemInQueue, void* arg)
{
	int nItem = *(int*)itemInQueue;
	int nArg = *(int*)arg;
	/*  0: found.
	 *  -: find L.
	 *  +: find R.  */
	return nArg-nItem;
}

static int _print (void* arg)
{
	mytag ("Travel", "arg=%d\n", *(int*)arg);
	return 0;
}

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


void sample_datatree ()
{
	int* pItem;
	VNode* p;
	VTree* Tree = vtree_create ();

	/* Insert. */
	p = Tree->insert (Tree, _lessThan, int_new(8));
	p = Tree->insert (Tree, _lessThan, int_new(4));
	p = Tree->insert (Tree, _lessThan, int_new(2));
	p = Tree->insert (Tree, _lessThan, int_new(6));
	p = Tree->insert (Tree, _lessThan, int_new(12));
	p = Tree->insert (Tree, _lessThan, int_new(10));
	p = Tree->insert (Tree, _lessThan, int_new(14));

	/* Travel. */
	Tree->travel (Tree, _print);
	
	/* Search. */
	p = Tree->search (Tree, _equal, pItem=int_new(7));
	if (p) {
		mytag ("Search", "id=[%2d], arg=%d  (Item=%d)\n", p->id, *(int*)p->arg, *pItem);
	} else {
		mytag ("Search", "none.  (Item=%d)\n", *pItem);
	}
	int_del (pItem);


	/* Delete. */
	Tree->lock (Tree);
	Tree->seek (Tree);
	while ((p = Tree->foreach (Tree, NULL, NULL)))
	{
		mytag ("Delete", "id=[%2d], arg=%d\n", p->id, *(int*)p->arg);
		Tree->delete (Tree, p, (dtor_ft)int_del);
	}
	Tree->unlock (Tree);

	vtree_destroy (Tree);
}



void sample_datalist ()
{
	int* pItem;
	VNode* p;
	VList* List = vlist_create ();

	/* Insert */
	p = List->insert (List, _lessThan, int_new(3));
	p = List->insert (List, _lessThan, int_new(2));
	p = List->insert (List, _lessThan, int_new(9));
	p = List->insert (List, _lessThan, int_new(6));
	p = List->insert (List, _lessThan, int_new(1));
	p = List->insert (List, _lessThan, int_new(4));

	/* Travel */
	List->travel (List, _print);

	/* Search */
	p = List->search (List, _equal, pItem=int_new(2));
	if (p) {
		mytag ("Search", "id=[%2d], arg=%d  (Item=%d)\n", p->id, *(int*)p->arg, *pItem);
	} else {
		mytag ("Search", "none.  (Item=%d)\n", *pItem);
	}
	int_del(pItem);

	/* Foreach & Delete */
	List->lock(List);
	List->seek (List);
	pItem=int_new(100);
	while((p = List->foreach (List, _lessThan, pItem)))
	{
		mytag ("Delete", "id=%2d, arg=%d\n", p->id, *(int*)p->arg);
		List->delete (List, p, (dtor_ft)int_del);
	}
	int_del(pItem);
	List->unlock(List);

	vlist_destroy (List);
	
	return ;
}





