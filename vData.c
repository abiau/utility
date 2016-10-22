
#include "vData.h"


VDataList_st* vdatalist_create  ()
{
	VDataList_st* pList = vc_malloc (sizeof(VDataList_st)); 
	if (pList==NULL)
	{
		return NULL;
	}

	memset (pList, 0, sizeof(VDataList_st));
	/* method. */
	pList->lock        = vdatalist_Lock;
	pList->unlock      = vdatalist_Unlock;
	pList->insert      = vdatalist_Insert;
	pList->search      = vdatalist_Search;
	pList->delete      = vdatalist_Delete;
	pList->seek        = vdatalist_Seek;
	pList->foreach     = vdatalist_Foreach;
	/* data. */
	pthread_mutex_init (&pList->m_mutex, NULL);

	return (VDataList_st*)pList;
}

void vdatalist_destroy (VDataList_st* pDataList)
{
	vc_free (pDataList, sizeof(VDataList_st));
	return ;
}

void vdatalist_Lock (void* self)
{
	VDataList_st* pList = (VDataList_st*) self;
	pthread_mutex_lock (&pList->m_mutex);
}
void vdatalist_Unlock (void* self) 
{
	VDataList_st* pList = (VDataList_st*) self;
	pthread_mutex_unlock (&pList->m_mutex);
}






u64t vdatanode_newId ()
{
	static u64t id=0;
	return  __sync_add_and_fetch (&id, 1);
}

VDataNode_st* vdatanode_create (void* arg)
{
	VDataNode_st* node = vc_malloc (sizeof(VDataNode_st));
	if (!node)
	{
		return NULL;
	}
	memset (node, 0, sizeof(VDataNode_st));
	node->arg = arg;
	node->id = vdatanode_newId();
	return node;
}

void vdatanode_destroy (VDataNode_st* node)
{
	vc_free (node, sizeof(VDataNode_st));
	node = NULL;
	return ;
}

int* int_new (int n)
{
	int* p = vc_malloc (sizeof(int));
	if (p)
	{
		*p = n;
	}
	return p;
}

void int_del (int* p)
{
	vc_free (p, sizeof(int));
}




VDataNode_st* vdatalist_Insert (void* self, validator v, void* arg)
{
	VDataList_st* pList = (VDataList_st*) self;
	VDataNode_st* node = vdatanode_create (arg);
	if (!node)
	{
		return NULL;
	}
	VDataNode_st* p = pList->head;
	if (v)
	{
		while (p)
		{
			if (v(node->arg, p->arg)==OK)
			{
				break;
			}
			else
			{
				p = p->next;
			}
		}

		if (p)
		{
			if (p->prev)
			{
				/*   ...N...   */
				node->prev = p->prev;
				node->next = p;
				p->prev->next = node;
				p->prev = node;
			}
			else
			{
				/*   N...   */
				node->next = p;
				p->prev = node;
				pList->head = node;
			}
		}
		else
		{
			if (pList->tail)
			{
				/*   ...N   */
				node->prev = pList->tail;
				pList->tail->next = node;
				pList->tail = node;
			}
			else
			{
				/*   N   */
				pList->head = node;
				pList->tail = node;
			}
		}
	}
	else
	{
		/*   ...N   */
		if (pList->tail)
		{
			/*   ...N   */
			node->prev = pList->tail;
			pList->tail->next = node;
			pList->tail = node;
		}
		else
		{
			/*   N   */
			pList->head = node;
			pList->tail = node;
		}
	}

	pList->size++;
	return node;
}

VDataNode_st* vdatalist_Search (void* self, validator v, void* arg)
{
	VDataList_st* pList = (VDataList_st*) self;
	VDataNode_st* node = pList->head;

	if (!v)
	{
		return NULL;
	}
	while (node)
	{
		if (v(arg, node->arg)==OK)
		{
			break;
		}
		else
		{
			node = node->next;
		}
	}

	return node;
}

void vdatalist_Delete (void* self, VDataNode_st* node, void_fn todo)
{
	VDataList_st* pList = (VDataList_st*) self;
	if (node)
	{
		if (node->prev)
		{
			node->prev->next = node->next;
		}
		else
		{
			pList->head = node->next;
		}

		if (node->next)
		{
			node->next->prev = node->prev;
		}
		else
		{
			pList->tail = node->prev;
		}
	}
	if (todo)
	{
		todo(node->arg);
	}
	vdatanode_destroy (node);
	
	return ;
}

void vdatalist_Seek (void* self, VDataNode_st* node)
{
	VDataList_st* pList = (VDataList_st*) self;
	if (node)
	{
		pList->cur = node;
	}
	else
	{
		pList->cur = pList->head;
	}
	return ;
}

VDataNode_st* vdatalist_Foreach (void* self)
{
	VDataList_st* pList = (VDataList_st*) self;
	VDataNode_st* node = pList->cur;
	if (pList->cur)
	{
		pList->cur = pList->cur->next;
	}

	return node;
}




