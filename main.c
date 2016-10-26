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

static int _SmallerThan (void* itemInQueue, void* arg);

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
#if 0
	p = Tree->insert (Tree, _SmallerThan, int_new(8));
	p = Tree->insert (Tree, _SmallerThan, int_new(4));
	p = Tree->insert (Tree, _SmallerThan, int_new(2));
	p = Tree->insert (Tree, _SmallerThan, int_new(6));
	p = Tree->insert (Tree, _SmallerThan, int_new(12));
	p = Tree->insert (Tree, _SmallerThan, int_new(10));
	p = Tree->insert (Tree, _SmallerThan, int_new(14));
#else
	p = Tree->insert (Tree, _SmallerThan, int_new(6));
	p = Tree->insert (Tree, _SmallerThan, int_new(4));
	p = Tree->insert (Tree, _SmallerThan, int_new(2));
#endif
	vdatatree_Travel (Tree, Tree->head, _do_print);
	p = vdatatree_FindMax (Tree, Tree->head);
	if (p) {
		vlog ("[FindMax] id=[%2d], arg=%d\n", p->id, *(int*)p->arg);
	} else {
		vlog ("[FindMax] none.\n");
	}
	p = vdatatree_Find (Tree, Tree->head, _equal, int_new(2));
	if (p) {
		vlog ("[Find] id=[%2d], arg=%d\n", p->id, *(int*)p->arg);
	} else {
		vlog ("[Find] none.\n");
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




