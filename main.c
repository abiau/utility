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

VLog* Log;

#define vlog(x, ...)        Log->print(   Log, __func__, __LINE__,    "      "    , x, ##__VA_ARGS__)

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

static int _equal (void* itemInQueue, void* arg)
{
	int nItem = *(int*)itemInQueue;
	int nArg = *(int*)arg;
	if (nItem == nArg)
	{
		/* Found. */
		return OK;
	}
	else if (nArg<nItem)
	{
		/* Find L. */
		return nArg-nItem;
	}
	else if (nArg>nItem)
	{
		/* Find R. */
		return nArg-nItem;
	}
}

static int _print (void* arg)
{
	vlog ("[TRAVEL]  arg=%d\n", *(int*)arg);
	return 0;
}

static int _smallerThan (void* itemInQueue, void* arg)
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

static int _increment (void* itemInQueue, void* arg)
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
	p = Tree->insert (Tree, _increment, int_new(8));
	p = Tree->insert (Tree, _increment, int_new(4));
	p = Tree->insert (Tree, _increment, int_new(2));
	p = Tree->insert (Tree, _increment, int_new(6));
	p = Tree->insert (Tree, _increment, int_new(12));
	p = Tree->insert (Tree, _increment, int_new(10));
	p = Tree->insert (Tree, _increment, int_new(14));
	Tree->travel (Tree, _print);
	
	/* Search. */
	p = Tree->search (Tree, _equal, pItem=int_new(7));
	if (p)
	{
		vlog ("[Search] id=[%2d], arg=%d\n", p->id, *(int*)p->arg);
	}
	else
	{
		vlog ("[Search] none.\n");
	}
	int_del (pItem);


	/* Delete. */
	Tree->seek (Tree, MAX);
	while (p = Tree->foreach (Tree, MAX, NULL, NULL))
	{
		Tree->delete (Tree, p, (dtor_ft)int_del);
		printf ("\n");
		Tree->travel (Tree, _print);
	}

	vdatatree_destroy (Tree);
}



void sample_datalist ()
{
	int* pItem;
	VDataNode* p;
	VDataList* List = vdatalist_create ();
	
	/* Insert */
	p = List->insert (List, HEAD, _increment, int_new(3));
	p = List->insert (List, HEAD, _increment, int_new(2));
	p = List->insert (List, HEAD, _increment, int_new(9));
	p = List->insert (List, HEAD, _increment, int_new(6));
	p = List->insert (List, HEAD, _increment, int_new(1));
	p = List->insert (List, HEAD, _increment, int_new(4));
	vlog ("[INFO] size=%d\n", List->size);
	List->travel (List, HEAD, _print);

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

	/* Foreach & Delete */
	List->seek (List, List->head);
	while(p = List->foreach (List, HEAD, _smallerThan, int_new(100)))
	{
		vlog ("[FOREACH] id=%2d, arg=%d\n", p->id, *(int*)p->arg);
		List->delete (List, p, (dtor_ft)int_del);
	}

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

#if 0
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




