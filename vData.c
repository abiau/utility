
#include "vData.h"


/*************************************************************/
/*************************************************************/

u64t vdatanode_newId ()
{
	static u64t id=0;
	return  __sync_add_and_fetch (&id, 1);
}

VDataNode* vdatanode_create (void* arg)
{
	VDataNode* node = vc_malloc (sizeof(VDataNode));
	if (!node)
	{
		return NULL;
	}
	memset (node, 0, sizeof(VDataNode));
	node->id = vdatanode_newId();
	node->arg = arg;
	return node;
}

void vdatanode_destroy (VDataNode* node)
{
	vc_free (node, sizeof(VDataNode));
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



/*************************************************************/
/*************************************************************/







VDataList* vdatalist_create  ()
{
	VDataList* pList = vc_malloc (sizeof(VDataList)); 
	if (pList==NULL)
	{
		return NULL;
	}

	memset (pList, 0, sizeof(VDataList));
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

	return (VDataList*)pList;
}

void vdatalist_destroy (VDataList* pList)
{
	vc_free (pList, sizeof(VDataList));
	return ;
}

void vdatalist_Lock (void* self)
{
	VDataList* pList = (VDataList*) self;
	pthread_mutex_lock (&pList->m_mutex);
}
void vdatalist_Unlock (void* self) 
{
	VDataList* pList = (VDataList*) self;
	pthread_mutex_unlock (&pList->m_mutex);
}








void _vdatalist_PushNode (void* self, SearchFrom_e from, VDataNode* node)
{
	VDataList* pList = (VDataList*) self;
	
	if (from==HEAD)
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
	else if (from==TAIL)
	{
		if (pList->head)
		{
			/*   N...   */
			node->next = pList->head;
			pList->head->prev = node;
			pList->head = node;
		}
		else
		{
			/*   N   */
			pList->head = node;
			pList->tail = node;
		}
	}

	return ;
}

VDataNode* vdatalist_Insert (void* self, SearchFrom_e from, comparator_ft comp, void* arg)
{
	VDataList* pList = (VDataList*) self;
	VDataNode* node = vdatanode_create (arg);
	if (!node)
	{
		return NULL;
	}
	pList->size++;

	if (!comp)
	{
		_vdatalist_PushNode (pList, from, node);
	}
	else
	{
		VDataNode* p = vdatalist_Search (pList, from, comp, arg);
		if (p)
		{
			if (from==HEAD)
			{
				if (p->prev)
				{
					/*   ...Np...   */
					node->prev = p->prev;
					node->next = p;
					p->prev->next = node;
					p->prev = node;
				}
				else
				{
					/*   Np...   */
					_vdatalist_PushNode (pList, HEAD, node);
				}
			}
			else if (from==TAIL)
			{
				if (p->next)
				{
					/*   ...pN...   */
					node->next = p->next;
					node->prev = p;
					p->next->prev = node;
					p->next = node;
				}
				else
				{
					/*   ..pN   */
					_vdatalist_PushNode (pList, TAIL, node);
				}
			}
		}
		else
		{
			if (from==HEAD)
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
					_vdatalist_PushNode (pList, HEAD, node);
				}
			}
			else if (from==TAIL)
			{
				if (pList->head)
				{
					/*   N...   */
					node->next = pList->head;
					pList->head->prev = node;
					pList->head = node;
				}
				else
				{
					/*   N   */
					_vdatalist_PushNode (pList, TAIL, node);
				}
			}
		}
	}

	return node;
}

VDataNode* vdatalist_Search (void* self, SearchFrom_e from, comparator_ft comp, void* arg)
{
	VDataList* pList = (VDataList*) self;
	if (!comp)
	{
		return NULL;
	}
	
	VDataNode* node;
	if (from==HEAD)
	{
		node = pList->head;
	}
	else if (from==TAIL)
	{
		node = pList->tail;
	}

	while (node)
	{
		if (comp(arg, node->arg)==OK)
		{
			break;
		}
		else
		{
			if (from==HEAD)
			{
				node = node->next;
			}
			else if (from==TAIL)
			{
				node = node->prev;
			}
		}
	}

	return node;
}

void vdatalist_Delete (void* self, VDataNode* node, destructor_ft del)
{
	VDataList* pList = (VDataList*) self;
	if (node)
	{
		if (pList->cur == node)
		{
			pList->cur = NULL;
		}
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
		if (del)
		{
			del(node->arg);
		}
		vdatanode_destroy (node);
	}
	return ;
}

void vdatalist_Seek (void* self, VDataNode* node)
{
	VDataList* pList = (VDataList*) self;
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

VDataNode* vdatalist_Foreach (void* self, SearchFrom_e from)
{
	VDataList* pList = (VDataList*) self;
	VDataNode* node = pList->cur;
	if (pList->cur)
	{
		if (from == HEAD)
		{
			pList->cur = pList->cur->next;
		}
		else
		{
			pList->cur = pList->cur->prev;
		}
	}

	return node;
}










/*************************************************************/
/*************************************************************/




VDataTree* vdatatree_create  ()
{
	VDataTree* pTree = vc_malloc (sizeof(VDataTree)); 
	if (pTree==NULL)
	{
		return NULL;
	}

	memset (pTree, 0, sizeof(VDataTree));
	/* method. */
	pTree->lock        = vdatatree_Lock;
	pTree->unlock      = vdatatree_Unlock;
	pTree->insert      = vdatatree_Insert;
	pTree->search      = vdatatree_Search;
	pTree->delete      = vdatatree_Delete;
	/* data. */
	pthread_mutex_init (&pTree->m_mutex, NULL);

	return (VDataTree*)pTree;
}

void vdatatree_destroy (VDataTree* pTree)
{
	vc_free (pTree, sizeof(VDataTree));
	return ;
}

void vdatatree_Lock (void* self)
{
	VDataTree* pTree = (VDataTree*) self;
	pthread_mutex_lock (&pTree->m_mutex);
}
void vdatatree_Unlock (void* self) 
{
	VDataTree* pTree = (VDataTree*) self;
	pthread_mutex_unlock (&pTree->m_mutex);
}





VDataNode* vdatatree_Insert (void* self, comparator_ft comp, void* arg)
{
	if (!comp)
	{
		return NULL;
	}
	VDataTree* pTree = (VDataTree*) self;
	VDataNode* node = vdatanode_create (arg);
	if (!node)
	{
		return NULL;
	}
	pTree->size++;

	if (!pTree->head)
	{
		pTree->head = node;
	}
	else
	{
#if 0
		VDataNode* p = vdatatree_Search (pTree, comp, arg);
		if (p)
		{
		}
		else
		{
		}
#endif
	}
	return node;
}

VDataNode* vdatatree_Search (void* self, comparator_ft comp, void* arg)
{
	if (!comp)
	{
		return NULL;
	}
	VDataTree* pTree = (VDataTree*) self;
	VDataNode* node;
	node = pTree->head;
	while (node)
	{
		int ret = comp(arg, node->arg);
		if (ret==OK)
		{
			break;
		}
		else if (ret<0)
		{
			node = node->L;
		}
		else if (ret>0)
		{
			node = node->R;
		}
	}
	return node;
}

void vdatatree_Delete (void* self, VDataNode* node, destructor_ft del)
{
	VDataTree* pTree = (VDataTree*) self;
	return ;
}










