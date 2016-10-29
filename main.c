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
	verr_init (3, "err.log", "./log/");
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
	vtag ("TRAVEL", "arg=%d\n", *(int*)arg);
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
	VDataNode* p;
	VDataTree* Tree = vdatatree_create ();

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
		vtag ("Search", "id=[%2d], arg=%d  (Item=%d)\n", p->id, *(int*)p->arg, *pItem);
	} else {
		vtag ("Search", "none.  (Item=%d)\n", *pItem);
	}
	int_del (pItem);


	/* Delete. */
	Tree->seek (Tree, MOST_R);
	while ((p = Tree->foreach (Tree, MOST_R, NULL, NULL)))
	{
		vtag ("Delete", "id=[%2d], arg=%d\n", p->id, *(int*)p->arg);
		Tree->delete (Tree, p, (dtor_ft)int_del);
	}

	vdatatree_destroy (Tree);
}



void sample_datalist ()
{
	int* pItem;
	VDataNode* p;
	VDataList* List = vdatalist_create ();
	
	/* Insert */
	p = List->insert (List, HEAD, _lessThan, int_new(3));
	p = List->insert (List, HEAD, _lessThan, int_new(2));
	p = List->insert (List, HEAD, _lessThan, int_new(9));
	p = List->insert (List, HEAD, _lessThan, int_new(6));
	p = List->insert (List, HEAD, _lessThan, int_new(1));
	p = List->insert (List, HEAD, _lessThan, int_new(4));

	/* Travel */
	List->travel (List, HEAD, _print);

	/* Search */
	p = List->search (List, HEAD, _equal, pItem=int_new(2));
	if (p) {
		vtag ("Search", "id=[%2d], arg=%d  (Item=%d)\n", p->id, *(int*)p->arg, *pItem);
	} else {
		vtag ("Search", "none.  (Item=%d)\n", *pItem);
	}
	int_del(pItem);

	/* Foreach & Delete */
	List->seek (List, List->head);
	pItem=int_new(100);
	while((p = List->foreach (List, HEAD, _lessThan, pItem)))
	{
		vtag ("Delete", "id=%2d, arg=%d\n", p->id, *(int*)p->arg);
		List->delete (List, p, (dtor_ft)int_del);
	}
	int_del(pItem);

	vdatalist_destroy (List);
	
	return ;
}



/*************************************************************************************/
/*************************************************************************************/

extern void sample_log ();
extern void sample_udpServer ();
extern void	sample_udpClient ();
extern void sample_tcpServer ();
extern void	sample_tcpClient ();

int main (int argc, char* argv[])
{

	init ();

#if 1
	sample_datatree ();
#else
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




